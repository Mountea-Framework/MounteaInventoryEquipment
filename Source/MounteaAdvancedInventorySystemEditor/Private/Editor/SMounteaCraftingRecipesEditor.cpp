// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#include "Editor/SMounteaCraftingRecipesEditor.h"
#include "Editor/MounteaInventoryEditorLayout.h"
#include "Editor/STemplateTreeItemRow.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Editor.h"
#include "FileHelpers.h"
#include "ObjectTools.h"
#include "ScopedTransaction.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Statics/MounteaAdvancedInventoryRecipeEditorStatics.h"
#include "Helpers/MounteaEditorNotificationHelpers.h"
#include "Framework/Application/SlateApplication.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Misc/MessageDialog.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Styling/AppStyle.h"
#include "UObject/UObjectGlobals.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SMounteaCraftingRecipesEditor"

namespace
{
	FText RecipeName(const UMounteaRecipeTemplate* Recipe)
	{
		if (!Recipe) return LOCTEXT("Invalid", "Invalid recipe");
		const auto* Result = Recipe->ResultItem.LoadSynchronous();
		if (!Result || Result->DisplayName.IsEmpty()) return FText::FromString(Recipe->GetName());
		return Result->DisplayName;
	}
	FString RecipeGroup(const UMounteaRecipeTemplate* Recipe)
	{
		const auto* Result = Recipe->ResultItem.LoadSynchronous();
		return Result ? Result->ItemCategory : FString();
	}
	void ShowError(const FString& Error)
	{
		FMounteaEditorNotificationHelpers::ShowNotification(Error, EMounteaEditorNotificationType::Error);
	}
}

void SMounteaCraftingRecipesEditor::Construct(const FArguments& InArgs)
{
	FDetailsViewArgs Args;
	Args.bUpdatesFromSelection = false;
	Args.bLockable = false;
	Args.bAllowSearch = true;
	Args.bShowOptions = false;
	Args.bShowPropertyMatrixButton = false;
	Args.bHideSelectionTip = true;
	Args.NotifyHook = this;
	Details = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor").CreateDetailView(Args);
	TSharedRef<SWidget> Browser = SAssignNew(Tree, STreeView<FTreeItem>)
		.TreeItemsSource(&Roots).SelectionMode(ESelectionMode::Multi)
		.OnGenerateRow(this, &SMounteaCraftingRecipesEditor::GenerateRow)
		.OnGetChildren(this, &SMounteaCraftingRecipesEditor::GetRecipeChildren)
		.OnSelectionChanged(this, &SMounteaCraftingRecipesEditor::SelectionChanged)
		.OnExpansionChanged_Lambda([this](FTreeItem Item, bool Expanded)
		{
			if (!bRefreshing && Item.IsValid() && !Item->Recipe.IsValid())
			{
				if (Expanded) CollapsedGroups.Remove(Item->Group); else CollapsedGroups.Add(Item->Group);
			}
		});
	ChildSlot[MounteaInventoryEditorLayout::MakeLayout(CreateToolbar(),
		MounteaInventoryEditorLayout::MakeBrowserDetails(Browser, Details.ToSharedRef(),
			TAttribute<float>::CreateLambda([this]() { return GetCachedGeometry().GetLocalSize().X * 0.20f; }),
			TAttribute<float>::CreateLambda([this]() { return GetCachedGeometry().GetLocalSize().X * 0.50f; })))];
	auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	AddedHandle = Registry.OnAssetAdded().AddSP(this, &SMounteaCraftingRecipesEditor::OnAssetChanged);
	RemovedHandle = Registry.OnAssetRemoved().AddSP(this, &SMounteaCraftingRecipesEditor::OnAssetChanged);
	UpdatedHandle = Registry.OnAssetUpdated().AddSP(this, &SMounteaCraftingRecipesEditor::OnAssetChanged);
	RenamedHandle = Registry.OnAssetRenamed().AddSP(this, &SMounteaCraftingRecipesEditor::OnAssetRenamed);
	ChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddSP(this, &SMounteaCraftingRecipesEditor::OnObjectChanged);
	GEditor->RegisterForUndo(this);
	RefreshRecipes();
}

SMounteaCraftingRecipesEditor::~SMounteaCraftingRecipesEditor()
{
	if (auto* Module = FModuleManager::GetModulePtr<FAssetRegistryModule>("AssetRegistry"))
	{
		auto& Registry = Module->Get();
		Registry.OnAssetAdded().Remove(AddedHandle);
		Registry.OnAssetRemoved().Remove(RemovedHandle);
		Registry.OnAssetUpdated().Remove(UpdatedHandle);
		Registry.OnAssetRenamed().Remove(RenamedHandle);
	}
	FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(ChangedHandle);
	if (GEditor) GEditor->UnregisterForUndo(this);
	if (Details) Details->SetObject(nullptr);
}

void SMounteaCraftingRecipesEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(Draft);
	Collector.AddReferencedObjects(Recipes);
}

TSharedRef<SWidget> SMounteaCraftingRecipesEditor::CreateToolbar()
{
	FMounteaInventoryEditorToolbarActions Actions;
	Actions.Create = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::CreateRecipe);
	Actions.Save = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::SaveCurrent);
	Actions.SaveAll = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::SaveAll);
	Actions.Import = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::ImportRecipes);
	Actions.Export = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::ExportRecipes);
	Actions.Close = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::CloseRecipe);
	Actions.Help = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::ShowHelp);
	Actions.CanSave = TAttribute<bool>::CreateLambda([this]()
	{ return Current.IsValid() && (Current.Get() == Draft || Current->GetPackage()->IsDirty()); });
	Actions.CanSaveAll = TAttribute<bool>::CreateLambda([this]() { return HasDirtyRecipes(); });
	return MounteaInventoryEditorLayout::MakeToolbar(Actions,
		SAssignNew(Search, SSearchBox).HintText(LOCTEXT("Search", "Search recipes, stations, categories..."))
		.OnTextChanged_Lambda([this](const FText& Text) { SearchText = Text.ToString(); RefreshRecipes(); }));
}

void SMounteaCraftingRecipesEditor::RefreshRecipes()
{
	TGuardValue<bool> Guard(bRefreshing, true);
	const TWeakObjectPtr<UMounteaRecipeTemplate> Selected = Current;
	TArray<TWeakObjectPtr<UMounteaRecipeTemplate>> SelectedRecipes;
	for (const auto& SelectedItem : Tree->GetSelectedItems())
		if (SelectedItem->Recipe.IsValid()) SelectedRecipes.Add(SelectedItem->Recipe);
	TArray<FAssetData> Assets;
	auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	Registry.GetAssetsByClass(UMounteaRecipeTemplate::StaticClass()->GetClassPathName(), Assets, true);
	Recipes.Reset();
	for (const FAssetData& Asset : Assets)
		if (auto* Recipe = Cast<UMounteaRecipeTemplate>(Asset.GetAsset()))
		{
			Recipe->SetFlags(RF_Transactional);
			Recipes.Add(Recipe);
		}
	if (Draft) Recipes.Add(Draft);
	Recipes.Sort([](const UMounteaRecipeTemplate& A, const UMounteaRecipeTemplate& B)
	{ return RecipeName(&A).ToString() < RecipeName(&B).ToString(); });
	Roots.Reset();
	TMap<FString, FTreeItem> Groups;
	for (UMounteaRecipeTemplate* Recipe : Recipes)
	{
		const FString Group = RecipeGroup(Recipe);
		const FString Searchable = RecipeName(Recipe).ToString() + TEXT(" ") + Group + TEXT(" ") + Recipe->RequiredCraftingPlace.ToString() + TEXT(" ") + Recipe->GetPathName() + TEXT(" ") + Recipe->RecipeGuid.ToString();
		if (!SearchText.IsEmpty() && !Searchable.Contains(SearchText)) continue;
		FTreeItem* Existing = Groups.Find(Group);
		if (!Existing)
		{
			FTreeItem NewGroup = MakeShared<FMounteaRecipeTreeItem>(); NewGroup->Group = Group;
			Roots.Add(NewGroup); Groups.Add(Group, NewGroup); Existing = Groups.Find(Group);
		}
		FTreeItem Row = MakeShared<FMounteaRecipeTreeItem>(); Row->Recipe = Recipe;
		(*Existing)->Children.Add(Row);
	}
	Roots.Sort([](const FTreeItem& A, const FTreeItem& B) { return A->Group < B->Group; });
	Tree->RequestTreeRefresh();
	for (const FTreeItem& Root : Roots) Tree->SetItemExpansion(Root, !CollapsedGroups.Contains(Root->Group));
	if (SelectedRecipes.Num() > 1)
	{
		TArray<UObject*> Objects;
		Tree->ClearSelection();
		for (const auto& Root : Roots)
			for (const auto& Child : Root->Children)
				if (Child->Recipe.IsValid() && SelectedRecipes.Contains(Child->Recipe))
				{
					Tree->SetItemSelection(Child, true, ESelectInfo::Direct);
					Objects.Add(Child->Recipe.Get());
				}
		Current = Objects.Num() == 1 ? Cast<UMounteaRecipeTemplate>(Objects[0]) : nullptr;
		Details->SetObjects(Objects);
	}
	else if (Selected.IsValid() && Recipes.Contains(Selected.Get())) SelectRecipe(Selected.Get());
	else if (!Current.IsValid() || !Recipes.Contains(Current.Get())) { Current.Reset(); Details->SetObject(nullptr); }
}

void SMounteaCraftingRecipesEditor::SelectRecipe(UMounteaRecipeTemplate* Recipe)
{
	TGuardValue<bool> Guard(bRefreshing, true);
	Current = Recipe;
	Details->SetObject(Recipe);
	Tree->ClearSelection();
	for (const FTreeItem& Root : Roots)
		for (const FTreeItem& Child : Root->Children)
			if (Child->Recipe.Get() == Recipe)
			{
				Tree->SetItemExpansion(Root, true);
				Tree->SetSelection(Child, ESelectInfo::Direct);
				return;
			}
}

void SMounteaCraftingRecipesEditor::GetRecipeChildren(FTreeItem Item, TArray<FTreeItem>& Children) const
{ if (Item) Children = Item->Children; }

void SMounteaCraftingRecipesEditor::SelectionChanged(FTreeItem Item, ESelectInfo::Type Type)
{
	if (bRefreshing) return;
	TArray<UObject*> Selection;
	for (const auto& Selected : Tree->GetSelectedItems())
		if (Selected->Recipe.IsValid()) Selection.Add(Selected->Recipe.Get());
	Current = Selection.Num() == 1 ? Cast<UMounteaRecipeTemplate>(Selection[0]) : nullptr;
	Details->SetObjects(Selection);
}

TSharedRef<ITableRow> SMounteaCraftingRecipesEditor::GenerateRow(FTreeItem Item, const TSharedRef<STableViewBase>& Owner)
{
	using SRecipeTreeItemRow = SMounteaInventoryTreeItemRow<FTreeItem>;
	if (!Item.IsValid()) return SNew(SRecipeTreeItemRow, Owner)[SNullWidget::NullWidget];
	if (!Item->Recipe.IsValid())
		return SNew(SRecipeTreeItemRow, Owner).Padding(FMargin(0, 4, 0, 2))
		[MounteaInventoryTreeRows::MakeCategoryContent(Item->Group,
			TAttribute<int32>::CreateLambda([Item]() { return Item->Children.Num(); }))];

	const TWeakObjectPtr<UMounteaRecipeTemplate> Recipe = Item->Recipe;
	FMounteaInventoryAssetRowPresentation Presentation;
	Presentation.Name = TAttribute<FText>::CreateLambda([this, Recipe]()
	{
		if (!Recipe.IsValid()) return FText::GetEmpty();
		return MounteaInventoryTreeRows::DisplayName(RecipeName(Recipe.Get()), Recipe.Get() == Draft || Recipe->GetPackage()->IsDirty());
	});
	Presentation.Tooltip = TAttribute<FText>::CreateLambda([Recipe]()
	{ return Recipe.IsValid() ? FText::FromString(Recipe->GetPathName()) : FText::GetEmpty(); });
	Presentation.AssetPath = MounteaInventoryTreeRows::AssetPath(Recipe.Get());
	Presentation.NameColor = TAttribute<FSlateColor>::CreateLambda([this, Recipe]()
	{
		if (Recipe.IsValid())
		{
			if (Recipe.Get() == Draft) return FSlateColor(FLinearColor(1.f, 1.f, 0.5f));
			if (Recipe->GetPackage()->IsDirty()) return FSlateColor(FLinearColor(1.f, 0.7f, 0.7f));
		}
		return FSlateColor::UseForeground();
	});
	Presentation.IsSelected = TAttribute<bool>::CreateLambda([this, Item]()
	{ return Tree.IsValid() && Tree->IsItemSelected(Item); });
	Presentation.ActionsVisibility = Recipe.Get() == Draft ? EVisibility::Collapsed : EVisibility::Visible;
	Presentation.Select = FOnClicked::CreateLambda([this, Item]()
	{ return MounteaInventoryTreeRows::SelectItem(Tree, Item, Roots); });
	Presentation.Locate = FOnClicked::CreateLambda([Recipe]()
	{ return MounteaInventoryTreeRows::LocateAsset(Recipe.Get()); });
	Presentation.Duplicate = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::DuplicateRecipe, Recipe);
	Presentation.Delete = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::DeleteRecipe, Recipe);
	return SNew(SRecipeTreeItemRow, Owner).Padding(FMargin(4, 2))
		[MounteaInventoryTreeRows::MakeAssetContent(Presentation)];
}

void SMounteaCraftingRecipesEditor::OnAssetChanged(const FAssetData& Asset)
{
	if (Asset.AssetClassPath != UMounteaRecipeTemplate::StaticClass()->GetClassPathName()) return;
	if (!bRefreshPending)
	{
		bRefreshPending = true;
		RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SMounteaCraftingRecipesEditor::DeferredRefresh));
	}
}
void SMounteaCraftingRecipesEditor::OnAssetRenamed(const FAssetData& Asset, const FString& OldPath) { OnAssetChanged(Asset); }
EActiveTimerReturnType SMounteaCraftingRecipesEditor::DeferredRefresh(double Time, float Delta)
{ bRefreshPending = false; RefreshRecipes(); return EActiveTimerReturnType::Stop; }

void SMounteaCraftingRecipesEditor::OnObjectChanged(UObject* Object, FPropertyChangedEvent& Event)
{
	auto* Recipe = Cast<UMounteaRecipeTemplate>(Object);
	if (!Recipe) return;
	if (Recipe == Draft) bDraftDirty = true;
	if (Recipes.Contains(Recipe) && Event.ChangeType != EPropertyChangeType::Interactive)
	{
		if (!bRefreshPending)
		{
			bRefreshPending = true;
			RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SMounteaCraftingRecipesEditor::DeferredRefresh));
		}
	}
}
void SMounteaCraftingRecipesEditor::NotifyPostChange(const FPropertyChangedEvent& Event, FProperty* Property)
{
	if (Current.Get() == Draft && Draft) bDraftDirty = true;
	Tree->RequestTreeRefresh();
}
void SMounteaCraftingRecipesEditor::PostUndo(bool bSuccess)
{
	if (!bSuccess) return;
	if (Draft) bDraftDirty = true;
	RefreshRecipes(); Details->ForceRefresh();
}

bool SMounteaCraftingRecipesEditor::HasDirtyRecipes() const
{
	if (Draft && bDraftDirty) return true;
	for (UMounteaRecipeTemplate* Recipe : Recipes) if (Recipe != Draft && IsValid(Recipe) && Recipe->GetPackage()->IsDirty()) return true;
	return false;
}

bool SMounteaCraftingRecipesEditor::ConfirmDiscardDraft()
{
	if (!Draft || !bDraftDirty) return true;
	const auto Answer = FMessageDialog::Open(EAppMsgType::YesNoCancel,
		LOCTEXT("DraftPrompt", "Save the unsaved recipe before closing it?"));
	if (Answer == EAppReturnType::Yes) return SaveRecipe(Draft);
	return Answer == EAppReturnType::No;
}

bool SMounteaCraftingRecipesEditor::CanCloseEditor()
{
	if (!HasDirtyRecipes()) return true;
	const auto Answer = FMessageDialog::Open(EAppMsgType::YesNoCancel,
		LOCTEXT("ClosePrompt", "Save modified recipes before closing the editor? Unsaved draft data will be lost if you choose No. Existing assets remain dirty in the project."));
	if (Answer == EAppReturnType::No) return true;
	if (Answer != EAppReturnType::Yes) return false;
	SaveAll(); return !HasDirtyRecipes();
}

FReply SMounteaCraftingRecipesEditor::CreateRecipe()
{
	if (!ConfirmDiscardDraft()) return FReply::Handled();
	const FScopedTransaction Transaction(LOCTEXT("NewRecipeTransaction", "Create crafting recipe"));
	Draft = NewObject<UMounteaRecipeTemplate>(GetTransientPackage(), NAME_None, RF_Transient | RF_Transactional);
	Draft->Modify();
	bDraftDirty = false;
	Search->SetText(FText::GetEmpty());
	RefreshRecipes(); SelectRecipe(Draft);
	return FReply::Handled();
}

bool SMounteaCraftingRecipesEditor::SaveRecipe(UMounteaRecipeTemplate* Recipe)
{
	FString Error;
	if (!UMounteaAdvancedInventoryRecipeEditorStatics::ValidateRecipeTemplate(Recipe, Error)) { ShowError(Error); return false; }
	if (Recipe == Draft)
	{
		FSaveAssetDialogConfig Config;
		Config.DialogTitleOverride = LOCTEXT("SaveTitle", "Save Crafting Recipe");
		Config.DefaultPath = TEXT("/Game"); Config.DefaultAssetName = TEXT("NewCraftingRecipe");
		Config.AssetClassNames.Add(UMounteaRecipeTemplate::StaticClass()->GetClassPathName());
		Config.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::Disallow;
		const FString ObjectPath = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get().CreateModalSaveAssetDialog(Config);
		if (ObjectPath.IsEmpty()) return false;
		const FString PackageName = FPackageName::ObjectPathToPackageName(ObjectPath);
		UPackage* Package = CreatePackage(*PackageName);
		const FScopedTransaction Transaction(LOCTEXT("SaveNewTransaction", "Create recipe asset"));
		auto* Asset = DuplicateObject<UMounteaRecipeTemplate>(Recipe, Package, *FPackageName::GetLongPackageAssetName(PackageName));
		Asset->ClearFlags(RF_Transient); Asset->SetFlags(RF_Public | RF_Standalone | RF_Transactional);
		// Saving a draft preserves its identity. Only DuplicateRecipe assigns a fresh GUID.
		Asset->RecipeGuid = Recipe->RecipeGuid;
		Asset->MarkPackageDirty(); FAssetRegistryModule::AssetCreated(Asset);
		Recipes.AddUnique(Asset); Draft = nullptr; bDraftDirty = false;
		Recipe = Asset; Current = Asset; Details->SetObject(Asset);
	}
	TArray<UPackage*> Packages; Packages.Add(Recipe->GetPackage());
	const bool Saved = FEditorFileUtils::PromptForCheckoutAndSave(Packages, false, false) == FEditorFileUtils::PR_Success;
	if (!Saved) ShowError(TEXT("Recipe could not be saved. Its changes remain available in the editor."));
	RefreshRecipes(); SelectRecipe(Recipe);
	return Saved;
}
FReply SMounteaCraftingRecipesEditor::SaveCurrent()
{ if (Current.IsValid()) SaveRecipe(Current.Get()); return FReply::Handled(); }
FReply SMounteaCraftingRecipesEditor::SaveAll()
{
	TArray<TWeakObjectPtr<UMounteaRecipeTemplate>> Dirty;
	for (UMounteaRecipeTemplate* Recipe : Recipes)
		if (Recipe && ((Recipe == Draft && bDraftDirty) || (Recipe != Draft && Recipe->GetPackage()->IsDirty()))) Dirty.Add(Recipe);
	// Validate every selected asset before the first package is written.
	for (const auto& Recipe : Dirty)
	{
		FString Error;
		if (Recipe.IsValid() && !UMounteaAdvancedInventoryRecipeEditorStatics::ValidateRecipeTemplate(Recipe.Get(), Error))
		{ ShowError(Error); return FReply::Handled(); }
	}
	for (const auto& Recipe : Dirty) if (Recipe.IsValid() && !SaveRecipe(Recipe.Get())) break;
	return FReply::Handled();
}
FReply SMounteaCraftingRecipesEditor::CloseRecipe()
{
	if (Current.Get() == Draft && !ConfirmDiscardDraft()) return FReply::Handled();
	if (Current.Get() == Draft) { Draft = nullptr; bDraftDirty = false; }
	Current.Reset(); Tree->ClearSelection(); Details->SetObject(nullptr); RefreshRecipes();
	return FReply::Handled();
}
FReply SMounteaCraftingRecipesEditor::DuplicateRecipe(TWeakObjectPtr<UMounteaRecipeTemplate> Recipe)
{
	if (!Recipe.IsValid() || !ConfirmDiscardDraft()) return FReply::Handled();
	const FScopedTransaction Transaction(LOCTEXT("DuplicateTransaction", "Duplicate crafting recipe"));
	Draft = DuplicateObject<UMounteaRecipeTemplate>(Recipe.Get(), GetTransientPackage(),
		MakeUniqueObjectName(GetTransientPackage(), Recipe->GetClass(), TEXT("CraftingRecipeCopy")));
	Draft->ClearFlags(RF_Public | RF_Standalone); Draft->SetFlags(RF_Transient | RF_Transactional);
	Draft->Modify(); Draft->RecipeGuid = FGuid::NewGuid(); Draft->SetSourceFilePath(FString());
	bDraftDirty = true; Search->SetText(FText::GetEmpty()); RefreshRecipes(); SelectRecipe(Draft);
	return FReply::Handled();
}
FReply SMounteaCraftingRecipesEditor::DeleteRecipe(TWeakObjectPtr<UMounteaRecipeTemplate> Recipe)
{
	if (!Recipe.IsValid()) return FReply::Handled();
	if (Recipe.Get() == Draft)
	{
		if (!ConfirmDiscardDraft()) return FReply::Handled();
		Draft = nullptr; bDraftDirty = false; Current.Reset(); Details->SetObject(nullptr);
	}
	else
	{
		// Release editor references so the normal asset deletion dialog can detect real referencers.
		auto* Asset = Recipe.Get(); Details->SetObject(nullptr); Current.Reset(); Recipes.Remove(Asset);
		TArray<UObject*> ToDelete; ToDelete.Add(Asset); ObjectTools::DeleteObjects(ToDelete, true);
	}
	RefreshRecipes(); return FReply::Handled();
}

FReply SMounteaCraftingRecipesEditor::ImportRecipes()
{
	IDesktopPlatform* Desktop = FDesktopPlatformModule::Get();
	if (!Desktop) { ShowError(TEXT("File dialogs are unavailable.")); return FReply::Handled(); }
	const void* Parent = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(AsShared());
	TArray<FString> Files;
	if (!Desktop->OpenFileDialog(Parent, TEXT("Import Crafting Recipes"), FString(), FString(),
		TEXT("Mountea Recipes (*.mntearecipe;*.mntearecipes)|*.mntearecipe;*.mntearecipes"), EFileDialogFlags::None, Files) || Files.IsEmpty()) return FReply::Handled();
	if (HasDirtyRecipes())
	{
		if (FMessageDialog::Open(EAppMsgType::OkCancel,
			LOCTEXT("ImportDirtyPrompt", "Import may update existing recipes. Save all modified recipes before importing?")) != EAppReturnType::Ok)
			return FReply::Handled();
		SaveAll();
		if (HasDirtyRecipes()) return FReply::Handled();
	}
	FString Folder = TEXT("/Game"); bool Accepted = false;
	TSharedRef<SWindow> Window = SNew(SWindow).Title(LOCTEXT("ImportFolder", "Choose Recipe Import Folder"))
		.ClientSize(FVector2D(500, 450)).SupportsMinimize(false).SupportsMaximize(false);
	TWeakPtr<SWindow> WeakWindow = Window;
	FPathPickerConfig Config; Config.DefaultPath = Folder; Config.bAllowReadOnlyFolders = false;
	Config.OnPathSelected = FOnPathSelected::CreateLambda([&Folder](const FString& Path) { Folder = Path; });
	Window->SetContent(SNew(SVerticalBox)
		+ SVerticalBox::Slot().FillHeight(1.f)[FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get().CreatePathPicker(Config)]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right).Padding(8.f)
		[SNew(SButton).Text(LOCTEXT("ImportHere", "Import Here")).OnClicked_Lambda([&Accepted, WeakWindow]() { Accepted = true; if (auto Pinned = WeakWindow.Pin()) Pinned->RequestDestroyWindow(); return FReply::Handled(); })]);
	FSlateApplication::Get().AddModalWindow(Window, FSlateApplication::Get().FindWidgetWindow(AsShared()));
	if (!Accepted) return FReply::Handled();
	TArray<UMounteaRecipeTemplate*> Imported; FString Error;
	if (!UMounteaAdvancedInventoryRecipeEditorStatics::ImportRecipesFromFilePath(Files[0], Folder, Imported, Error)) ShowError(Error);
	else
	{
		RefreshRecipes(); if (!Imported.IsEmpty()) SelectRecipe(Imported[0]);
		FMounteaEditorNotificationHelpers::ShowNotification(FString::Printf(TEXT("Imported %d recipes."), Imported.Num()), EMounteaEditorNotificationType::Success);
	}
	return FReply::Handled();
}

FReply SMounteaCraftingRecipesEditor::ExportRecipes()
{
	TArray<UMounteaRecipeTemplate*> Selected;
	for (const auto& Item : Tree->GetSelectedItems()) if (Item->Recipe.IsValid()) Selected.Add(Item->Recipe.Get());
	if (Selected.IsEmpty()) { ShowError(TEXT("Select at least one recipe to export.")); return FReply::Handled(); }
	IDesktopPlatform* Desktop = FDesktopPlatformModule::Get();
	if (!Desktop) { ShowError(TEXT("File dialogs are unavailable.")); return FReply::Handled(); }
	TArray<FString> Files;
	const bool Single = Selected.Num() == 1;
	const FString Extension = Single ? TEXT("mntearecipe") : TEXT("mntearecipes");
	if (Desktop->SaveFileDialog(FSlateApplication::Get().FindBestParentWindowHandleForDialogs(AsShared()), TEXT("Export Crafting Recipes"), FString(),
		TEXT("recipes.") + Extension, Single ? TEXT("Mountea Recipe (*.mntearecipe)|*.mntearecipe") : TEXT("Mountea Recipes (*.mntearecipes)|*.mntearecipes"), EFileDialogFlags::None, Files) && !Files.IsEmpty())
	{
		FString Error;
		const FString File = FPaths::ChangeExtension(Files[0], Extension);
		if (!UMounteaAdvancedInventoryRecipeEditorStatics::ExportRecipesToFilePath(Selected, File, Error)) ShowError(Error);
		else FMounteaEditorNotificationHelpers::ShowNotification(TEXT("Recipes exported successfully."), EMounteaEditorNotificationType::Success);
	}
	return FReply::Handled();
}

FReply SMounteaCraftingRecipesEditor::ShowHelp()
{
	TSharedRef<SWindow> Window = SNew(SWindow).Title(LOCTEXT("HelpTitle", "Crafting Recipes Editor Tutorial"))
		.ClientSize(FVector2D(660, 380)).SupportsMinimize(false).SupportsMaximize(false);
	Window->SetContent(SNew(SVerticalBox)
		+ SVerticalBox::Slot().FillHeight(1.f).Padding(20.f)
		[SNew(STextBlock).AutoWrapText(true).Text(LOCTEXT("HelpText",
			"Use New Template to create a recipe, select its result item, quantity, crafting time and optional station tag. Add ingredients directly in Recipe Ingredients: each entry has an Ingredient Source and Required Quantity. Every ingredient is required.\n\nRecipes are grouped by their result item category. Search matches result names, station tags, categories, asset paths and recipe GUIDs. Select several recipes to edit common properties or export a bundle.\n\nSave Template saves the current recipe; Save All Templates saves modified recipes. Duplicate creates a draft with a new GUID. Saving preserves the GUID. Delete uses the standard asset deletion dialog. Existing asset edits support Undo and Redo.\n\nThe Allowed Recipes list in your crafting configuration controls which recipes are available at runtime. Import and export use .mntearecipe and .mntearecipes files."))]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right).Padding(12.f)
		[SNew(SButton).Text(LOCTEXT("Website", "Open Mountea Website")).OnClicked_Lambda([]()
			{ FPlatformProcess::LaunchURL(TEXT("https://mountea.tools"), nullptr, nullptr); return FReply::Handled(); })]);
	FSlateApplication::Get().AddModalWindow(Window, FSlateApplication::Get().FindWidgetWindow(AsShared()));
	return FReply::Handled();
}
#undef LOCTEXT_NAMESPACE
