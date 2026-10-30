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
		if (!Recipe)
			return LOCTEXT("Invalid", "Invalid recipe");

		const auto* result = Recipe->ResultItem.LoadSynchronous();

		if (!result || result->DisplayName.IsEmpty())
			return FText::FromString(Recipe->GetName());

		return result->DisplayName;
	}
	FString RecipeGroup(const UMounteaRecipeTemplate* Recipe)
	{
		const auto* result = Recipe->ResultItem.LoadSynchronous();
		return result ? result->ItemCategory : FString();
	}
	void ShowError(const FString& Error)
	{
		FMounteaEditorNotificationHelpers::ShowNotification(Error, EMounteaEditorNotificationType::Error);
	}
}

void SMounteaCraftingRecipesEditor::Construct(const FArguments& InArgs)
{
	FDetailsViewArgs args;
	args.bUpdatesFromSelection = false;
	args.bLockable = false;
	args.bAllowSearch = true;
	args.bShowOptions = false;
	args.bShowPropertyMatrixButton = false;
	args.bHideSelectionTip = true;
	args.NotifyHook = this;
	Details = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor").CreateDetailView(args);
	TSharedRef<SWidget> browser = SAssignNew(Tree, STreeView<FTreeItem>)
		.TreeItemsSource(&Roots)
		.SelectionMode(ESelectionMode::Multi)
		.OnGenerateRow(this, &SMounteaCraftingRecipesEditor::GenerateRow)
		.OnGetChildren(this, &SMounteaCraftingRecipesEditor::GetRecipeChildren)
		.OnSelectionChanged(this, &SMounteaCraftingRecipesEditor::SelectionChanged)
		.OnExpansionChanged_Lambda([this](FTreeItem Item, bool Expanded)
		{
			if (!bRefreshing && Item.IsValid() && !Item->Recipe.IsValid())
			{
				if (Expanded)
					CollapsedGroups.Remove(Item->Group);
				else
					CollapsedGroups.Add(Item->Group);
			}
		});
	ChildSlot[MounteaInventoryEditorLayout::MakeLayout(CreateToolbar(),
		MounteaInventoryEditorLayout::MakeBrowserDetails(browser, Details.ToSharedRef(),
			TAttribute<float>::CreateLambda([this]()
			{
				return GetCachedGeometry().GetLocalSize().X * 0.20f;
			}),
			TAttribute<float>::CreateLambda([this]()
			{
				return GetCachedGeometry().GetLocalSize().X * 0.50f;
			})))];
	auto& registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	AddedHandle = registry.OnAssetAdded().AddSP(this, &SMounteaCraftingRecipesEditor::OnAssetChanged);
	RemovedHandle = registry.OnAssetRemoved().AddSP(this, &SMounteaCraftingRecipesEditor::OnAssetChanged);
	UpdatedHandle = registry.OnAssetUpdated().AddSP(this, &SMounteaCraftingRecipesEditor::OnAssetChanged);
	RenamedHandle = registry.OnAssetRenamed().AddSP(this, &SMounteaCraftingRecipesEditor::OnAssetRenamed);
	ChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddSP(this, &SMounteaCraftingRecipesEditor::OnObjectChanged);
	GEditor->RegisterForUndo(this);
	this->InitializeDraft();
	this->RefreshRecipes();
	this->SelectRecipe(this->Draft);
}

SMounteaCraftingRecipesEditor::~SMounteaCraftingRecipesEditor()
{
	if (auto* module = FModuleManager::GetModulePtr<FAssetRegistryModule>("AssetRegistry"))
	{
		auto& registry = module->Get();
		registry.OnAssetAdded().Remove(AddedHandle);
		registry.OnAssetRemoved().Remove(RemovedHandle);
		registry.OnAssetUpdated().Remove(UpdatedHandle);
		registry.OnAssetRenamed().Remove(RenamedHandle);
	}

	FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(ChangedHandle);

	if (GEditor)
		GEditor->UnregisterForUndo(this);

	if (Details)
		Details->SetObject(nullptr);
}

void SMounteaCraftingRecipesEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(Draft);
	Collector.AddReferencedObjects(Recipes);
}

TSharedRef<SWidget> SMounteaCraftingRecipesEditor::CreateToolbar()
{
	FMounteaInventoryEditorToolbarActions actions;
	actions.Create = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::CreateRecipe);
	actions.Save = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::SaveCurrent);
	actions.SaveAll = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::SaveAll);
	actions.Import = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::ImportRecipes);
	actions.Export = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::ExportRecipes);
	actions.Close = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::CloseRecipe);
	actions.Help = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::ShowHelp);
	actions.CanSave = TAttribute<bool>::CreateLambda([this]()
	{
		return Current.IsValid() && (Current.Get() == Draft || Current->GetPackage()->IsDirty());
	});
	actions.CanSaveAll = TAttribute<bool>::CreateLambda([this]()
	{
		return HasDirtyRecipes();
	});
	return MounteaInventoryEditorLayout::MakeToolbar(actions,
		SAssignNew(Search, SSearchBox).HintText(LOCTEXT("Search", "Search recipes, stations, categories..."))
		.OnTextChanged_Lambda([this](const FText& Text)
		{
			SearchText = Text.ToString();
			RefreshRecipes();
		}));
}

void SMounteaCraftingRecipesEditor::RefreshRecipes()
{
	TGuardValue<bool> guard(bRefreshing, true);
	const TWeakObjectPtr<UMounteaRecipeTemplate> selected = Current;
	TArray<TWeakObjectPtr<UMounteaRecipeTemplate>> selectedRecipes;
	for (const auto& selectedItem : Tree->GetSelectedItems())
	{
		if (selectedItem->Recipe.IsValid())
			selectedRecipes.Add(selectedItem->Recipe);
	}

	TArray<FAssetData> assets;
	auto& registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	registry.GetAssetsByClass(UMounteaRecipeTemplate::StaticClass()->GetClassPathName(), assets, true);
	Recipes.Reset();
	for (const FAssetData& asset : assets)
	{
		if (auto* recipe = Cast<UMounteaRecipeTemplate>(asset.GetAsset()))
		{
			if (recipe->HasAnyFlags(RF_Transient))
				continue;

			recipe->SetFlags(RF_Transactional);
			Recipes.Add(recipe);
		}
	}

	Recipes.Sort([](const UMounteaRecipeTemplate& A, const UMounteaRecipeTemplate& B)
	{
		return RecipeName(&A).ToString() < RecipeName(&B).ToString();
	});
	Roots.Reset();
	TMap<FString, FTreeItem> groups;
	for (UMounteaRecipeTemplate* recipe : Recipes)
	{
		const FString group = RecipeGroup(recipe);
		const FString searchable = RecipeName(recipe).ToString() + TEXT(" ") + group + TEXT(" ") + recipe->RequiredCraftingPlace.ToString() + TEXT(" ") + recipe->GetPathName() + TEXT(" ") + recipe->RecipeGuid.ToString();

		if (!SearchText.IsEmpty() && !searchable.Contains(SearchText))
			continue;

		FTreeItem* existing = groups.Find(group);

		if (!existing)
		{
			FTreeItem newGroup = MakeShared<FMounteaRecipeTreeItem>();
			newGroup->Group = group;
			Roots.Add(newGroup);
			groups.Add(group, newGroup);
			existing = groups.Find(group);
		}

		FTreeItem row = MakeShared<FMounteaRecipeTreeItem>();
		row->Recipe = recipe;
		(*existing)->Children.Add(row);
	}
	Roots.Sort([](const FTreeItem& A, const FTreeItem& B)
	{
		return A->Group < B->Group;
	});
	Tree->RequestTreeRefresh();
	for (const FTreeItem& root : Roots)
	{
		Tree->SetItemExpansion(root, !CollapsedGroups.Contains(root->Group));
	}

	if (selectedRecipes.Num() > 1)
	{
		TArray<UObject*> objects;
		Tree->ClearSelection();
		for (const auto& root : Roots)
		{
			for (const auto& child : root->Children)
			{
				if (child->Recipe.IsValid() && selectedRecipes.Contains(child->Recipe))
				{
					Tree->SetItemSelection(child, true, ESelectInfo::Direct);
					objects.Add(child->Recipe.Get());
				}
			}
		}

		Current = objects.Num() == 1 ? Cast<UMounteaRecipeTemplate>(objects[0]) : nullptr;
		Details->SetObjects(objects);
	}
	else if (selected.IsValid() && (selected.Get() == this->Draft || this->Recipes.Contains(selected.Get())))
		SelectRecipe(selected.Get());
	else if (!Current.IsValid() || !Recipes.Contains(Current.Get()))
	{
		Current.Reset();
		Details->SetObject(nullptr);
	}
}

void SMounteaCraftingRecipesEditor::SelectRecipe(UMounteaRecipeTemplate* Recipe)
{
	TGuardValue<bool> guard(bRefreshing, true);
	Current = Recipe;
	Details->SetObject(Recipe);
	Tree->ClearSelection();
	for (const FTreeItem& root : Roots)
	{
		for (const FTreeItem& child : root->Children)
		{
			if (child->Recipe.Get() == Recipe)
			{
				Tree->SetItemExpansion(root, true);
				Tree->SetSelection(child, ESelectInfo::Direct);
				return;
			}
		}
	}
}

void SMounteaCraftingRecipesEditor::GetRecipeChildren(FTreeItem Item, TArray<FTreeItem>& Children) const
{
	if (Item)
		Children = Item->Children;
}

void SMounteaCraftingRecipesEditor::SelectionChanged(FTreeItem Item, ESelectInfo::Type Type)
{
	if (this->bRefreshing)
		return;

	TArray<UObject*> selection;
	for (const auto& selected : this->Tree->GetSelectedItems())
	{
		if (selected->Recipe.IsValid())
			selection.Add(selected->Recipe.Get());
	}

	if (this->Current.Get() == this->Draft && this->Draft && this->bDraftDirty)
	{
		if (!this->ConfirmDiscardDraft())
		{
			this->SelectRecipe(this->Draft);
			return;
		}

		this->bDraftDirty = false;
	}

	if (selection.IsEmpty())
	{
		this->SelectRecipe(this->Draft);
		return;
	}

	if (selection.Num() == 1)
	{
		this->SelectRecipe(Cast<UMounteaRecipeTemplate>(selection[0]));
		return;
	}

	TGuardValue<bool> guard(this->bRefreshing, true);
	this->Tree->ClearSelection();
	for (const FTreeItem& root : this->Roots)
	{
		for (const FTreeItem& child : root->Children)
		{
			if (selection.Contains(child->Recipe.Get()))
				this->Tree->SetItemSelection(child, true, ESelectInfo::Direct);
		}
	}

	this->Current.Reset();
	this->Details->SetObjects(selection);
}

TSharedRef<ITableRow> SMounteaCraftingRecipesEditor::GenerateRow(FTreeItem Item, const TSharedRef<STableViewBase>& Owner)
{
	using SRecipeTreeItemRow = SMounteaInventoryTreeItemRow<FTreeItem>;

	if (!Item.IsValid())
		return SNew(SRecipeTreeItemRow, Owner)
		[
			SNullWidget::NullWidget
		];

	if (!Item->Recipe.IsValid())
		return SNew(SRecipeTreeItemRow, Owner)
			.Padding(FMargin(0, 4, 0, 2))
			[
				MounteaInventoryTreeRows::MakeCategoryContent(Item->Group,
			TAttribute<int32>::CreateLambda([Item]()
			{
				return Item->Children.Num();
			}))
			];

	const TWeakObjectPtr<UMounteaRecipeTemplate> recipe = Item->Recipe;
	FMounteaInventoryAssetRowPresentation presentation;
	presentation.Name = TAttribute<FText>::CreateLambda([this, recipe]()
	{
		if (!recipe.IsValid())
			return FText::GetEmpty();

		return MounteaInventoryTreeRows::DisplayName(RecipeName(recipe.Get()), recipe.Get() == Draft || recipe->GetPackage()->IsDirty());
	});
	presentation.Tooltip = TAttribute<FText>::CreateLambda([recipe]()
	{
		return recipe.IsValid() ? FText::FromString(recipe->GetPathName()) : FText::GetEmpty();
	});
	presentation.AssetPath = MounteaInventoryTreeRows::AssetPath(recipe.Get());
	presentation.NameColor = TAttribute<FSlateColor>::CreateLambda([this, recipe]()
	{
		if (recipe.IsValid())
		{
			if (recipe.Get() == Draft)
				return FSlateColor(FLinearColor(1.f, 1.f, 0.5f));

			if (recipe->GetPackage()->IsDirty())
				return FSlateColor(FLinearColor(1.f, 0.7f, 0.7f));
		}

		return FSlateColor::UseForeground();
	});
	presentation.IsSelected = TAttribute<bool>::CreateLambda([this, Item]()
	{
		return Tree.IsValid() && Tree->IsItemSelected(Item);
	});
	presentation.ActionsVisibility = recipe.Get() == Draft ? EVisibility::Collapsed : EVisibility::Visible;
	presentation.Select = FOnClicked::CreateLambda([this, Item]()
	{
		return MounteaInventoryTreeRows::SelectItem(Tree, Item, Roots);
	});
	presentation.Locate = FOnClicked::CreateLambda([recipe]()
	{
		return MounteaInventoryTreeRows::LocateAsset(recipe.Get());
	});
	presentation.Duplicate = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::DuplicateRecipe, recipe);
	presentation.Delete = FOnClicked::CreateSP(this, &SMounteaCraftingRecipesEditor::DeleteRecipe, recipe);
	return SNew(SRecipeTreeItemRow, Owner)
		.Padding(FMargin(4, 2))
		[
			MounteaInventoryTreeRows::MakeAssetContent(presentation)
		];
}

void SMounteaCraftingRecipesEditor::OnAssetChanged(const FAssetData& Asset)
{
	if (Asset.AssetClassPath != UMounteaRecipeTemplate::StaticClass()->GetClassPathName())
		return;

	if (!bRefreshPending)
	{
		bRefreshPending = true;
		RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SMounteaCraftingRecipesEditor::DeferredRefresh));
	}
}
void SMounteaCraftingRecipesEditor::OnAssetRenamed(const FAssetData& Asset, const FString& OldPath)
{
	OnAssetChanged(Asset);
}
EActiveTimerReturnType SMounteaCraftingRecipesEditor::DeferredRefresh(double Time, float Delta)
{
	bRefreshPending = false;
	RefreshRecipes();
	return EActiveTimerReturnType::Stop;
}

void SMounteaCraftingRecipesEditor::OnObjectChanged(UObject* Object, FPropertyChangedEvent& Event)
{
	auto* recipe = Cast<UMounteaRecipeTemplate>(Object);

	if (!recipe)
		return;

	if (recipe == Draft)
		bDraftDirty = true;

	if (Recipes.Contains(recipe) && Event.ChangeType != EPropertyChangeType::Interactive)
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
	if (Current.Get() == Draft && Draft)
		bDraftDirty = true;

	Tree->RequestTreeRefresh();
}
void SMounteaCraftingRecipesEditor::PostUndo(bool bSuccess)
{
	if (!bSuccess)
		return;

	if (Draft)
		bDraftDirty = true;

	RefreshRecipes();
	Details->ForceRefresh();
}

bool SMounteaCraftingRecipesEditor::HasDirtyRecipes() const
{
	if (Draft && bDraftDirty)
		return true;

	for (UMounteaRecipeTemplate* recipe : Recipes)
	{
		if (recipe != Draft && IsValid(recipe) && recipe->GetPackage()->IsDirty())
			return true;
	}

	return false;
}

bool SMounteaCraftingRecipesEditor::ConfirmDiscardDraft()
{
	if (!Draft || !bDraftDirty)
		return true;

	const auto answer = FMessageDialog::Open(EAppMsgType::YesNoCancel,
		LOCTEXT("DraftPrompt", "Save the unsaved recipe before closing it?"));

	if (answer == EAppReturnType::Yes)
		return SaveRecipe(Draft);

	return answer == EAppReturnType::No;
}

bool SMounteaCraftingRecipesEditor::CanCloseEditor()
{
	if (!HasDirtyRecipes())
		return true;

	const auto answer = FMessageDialog::Open(EAppMsgType::YesNoCancel,
		LOCTEXT("ClosePrompt", "Save modified recipes before closing the editor? Unsaved draft data will be lost if you choose No. Existing assets remain dirty in the project."));

	if (answer == EAppReturnType::No)
		return true;

	if (answer != EAppReturnType::Yes)
		return false;

	SaveAll();
	return !HasDirtyRecipes();
}

void SMounteaCraftingRecipesEditor::InitializeDraft()
{
	this->Draft = NewObject<UMounteaRecipeTemplate>(GetTransientPackage(), NAME_None, RF_Transient | RF_Transactional);
	this->bDraftDirty = false;
}

FReply SMounteaCraftingRecipesEditor::CreateRecipe()
{
	if (!ConfirmDiscardDraft())
		return FReply::Handled();

	const FScopedTransaction transaction(LOCTEXT("NewRecipeTransaction", "Create crafting recipe"));
	this->InitializeDraft();
	this->Draft->Modify();
	Search->SetText(FText::GetEmpty());
	RefreshRecipes();
	SelectRecipe(Draft);
	return FReply::Handled();
}

bool SMounteaCraftingRecipesEditor::SaveRecipe(UMounteaRecipeTemplate* Recipe)
{
	FString error;

	if (!UMounteaAdvancedInventoryRecipeEditorStatics::ValidateRecipeTemplate(Recipe, error))
	{
		ShowError(error);
		return false;
	}

	if (Recipe == Draft)
	{
		FSaveAssetDialogConfig config;
		config.DialogTitleOverride = LOCTEXT("SaveTitle", "Save Crafting Recipe");
		config.DefaultPath = TEXT("/Game");
		config.DefaultAssetName = TEXT("NewCraftingRecipe");
		config.AssetClassNames.Add(UMounteaRecipeTemplate::StaticClass()->GetClassPathName());
		config.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::Disallow;
		const FString objectPath = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get().CreateModalSaveAssetDialog(config);

		if (objectPath.IsEmpty())
			return false;

		const FString packageName = FPackageName::ObjectPathToPackageName(objectPath);
		UPackage* package = CreatePackage(*packageName);
		const FScopedTransaction transaction(LOCTEXT("SaveNewTransaction", "Create recipe asset"));
		auto* asset = DuplicateObject<UMounteaRecipeTemplate>(Recipe, package, *FPackageName::GetLongPackageAssetName(packageName));
		asset->ClearFlags(RF_Transient);
		asset->SetFlags(RF_Public | RF_Standalone | RF_Transactional);
		// Saving a draft preserves its identity. Only DuplicateRecipe assigns a fresh GUID.
		asset->RecipeGuid = Recipe->RecipeGuid;
		asset->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(asset);
		Recipes.AddUnique(asset);
		Draft = nullptr;
		bDraftDirty = false;
		Recipe = asset;
		Current = asset;
		Details->SetObject(asset);
	}

	TArray<UPackage*> packages;
	packages.Add(Recipe->GetPackage());

	const bool saved = FEditorFileUtils::PromptForCheckoutAndSave(packages, false, false) == FEditorFileUtils::PR_Success;

	if (!saved)
		ShowError(TEXT("Recipe could not be saved. Its changes remain available in the editor."));

	RefreshRecipes();
	SelectRecipe(Recipe);
	return saved;
}
FReply SMounteaCraftingRecipesEditor::SaveCurrent()
{
	if (Current.IsValid())
		SaveRecipe(Current.Get());
	return FReply::Handled();
}
FReply SMounteaCraftingRecipesEditor::SaveAll()
{
	TArray<TWeakObjectPtr<UMounteaRecipeTemplate>> dirty;

	if (this->Draft && this->bDraftDirty)
		dirty.Add(this->Draft);

	for (UMounteaRecipeTemplate* recipe : Recipes)
	{
		if (recipe && recipe->GetPackage()->IsDirty())
			dirty.Add(recipe);
	}

	// Validate every selected asset before the first package is written.
	for (const auto& recipe : dirty)
	{
		FString error;

		if (recipe.IsValid() && !UMounteaAdvancedInventoryRecipeEditorStatics::ValidateRecipeTemplate(recipe.Get(), error))
		{
			ShowError(error);
			return FReply::Handled();
		}
	}
	for (const auto& recipe : dirty)
	{
		if (recipe.IsValid() && !SaveRecipe(recipe.Get()))
			break;
	}

	return FReply::Handled();
}
FReply SMounteaCraftingRecipesEditor::CloseRecipe()
{
	if (Current.Get() == Draft && !ConfirmDiscardDraft())
		return FReply::Handled();

	if (Current.Get() == Draft)
	{
		Draft = nullptr;
		bDraftDirty = false;
	}

	Current.Reset();
	Tree->ClearSelection();
	Details->SetObject(nullptr);
	RefreshRecipes();

	return FReply::Handled();
}
FReply SMounteaCraftingRecipesEditor::DuplicateRecipe(TWeakObjectPtr<UMounteaRecipeTemplate> Recipe)
{
	if (!Recipe.IsValid() || !ConfirmDiscardDraft())
		return FReply::Handled();

	const FScopedTransaction transaction(LOCTEXT("DuplicateTransaction", "Duplicate crafting recipe"));
	Draft = DuplicateObject<UMounteaRecipeTemplate>(Recipe.Get(), GetTransientPackage(),
		MakeUniqueObjectName(GetTransientPackage(), Recipe->GetClass(), TEXT("CraftingRecipeCopy")));
	Draft->ClearFlags(RF_Public | RF_Standalone);
	Draft->SetFlags(RF_Transient | RF_Transactional);
	Draft->Modify();
	Draft->RecipeGuid = FGuid::NewGuid();
	Draft->SetSourceFilePath(FString());
	bDraftDirty = true;
	Search->SetText(FText::GetEmpty());
	RefreshRecipes();
	SelectRecipe(Draft);
	return FReply::Handled();
}
FReply SMounteaCraftingRecipesEditor::DeleteRecipe(TWeakObjectPtr<UMounteaRecipeTemplate> Recipe)
{
	if (!Recipe.IsValid())
		return FReply::Handled();

	if (Recipe.Get() == Draft)
	{
		if (!ConfirmDiscardDraft())
			return FReply::Handled();

		Draft = nullptr;
		bDraftDirty = false;
		Current.Reset();
		Details->SetObject(nullptr);
	}
	else
	{
		// Release editor references so the normal asset deletion dialog can detect real referencers.
		auto* asset = Recipe.Get();
		Details->SetObject(nullptr);
		Current.Reset();
		Recipes.Remove(asset);
		TArray<UObject*> toDelete;
		toDelete.Add(asset);
		ObjectTools::DeleteObjects(toDelete, true);
	}
	RefreshRecipes();
	return FReply::Handled();
}

FReply SMounteaCraftingRecipesEditor::ImportRecipes()
{
	IDesktopPlatform* desktop = FDesktopPlatformModule::Get();

	if (!desktop)
	{
		ShowError(TEXT("File dialogs are unavailable."));
		return FReply::Handled();
	}

	const void* parent = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(AsShared());

	TArray<FString> files;

	if (!desktop->OpenFileDialog(parent, TEXT("Import Crafting Recipes"), FString(), FString(),
		TEXT("Mountea Recipes (*.mntearecipe;*.mntearecipes)|*.mntearecipe;*.mntearecipes"), EFileDialogFlags::None, files) || files.IsEmpty())
    {
        return FReply::Handled();
    }

	if (HasDirtyRecipes())
	{
		if (FMessageDialog::Open(EAppMsgType::OkCancel,
			LOCTEXT("ImportDirtyPrompt", "Import may update existing recipes. Save all modified recipes before importing?")) != EAppReturnType::Ok)
			return FReply::Handled();

		SaveAll();

		if (HasDirtyRecipes())
			return FReply::Handled();
	}

	FString folder = TEXT("/Game");
	bool accepted = false;
	TSharedRef<SWindow> window = SNew(SWindow)
		.Title(LOCTEXT("ImportFolder", "Choose Recipe Import Folder"))
		.ClientSize(FVector2D(500, 450))
		.SupportsMinimize(false)
		.SupportsMaximize(false);
	TWeakPtr<SWindow> weakWindow = window;
	FPathPickerConfig config;
	config.DefaultPath = folder;
	config.bAllowReadOnlyFolders = false;
	config.OnPathSelected = FOnPathSelected::CreateLambda([&folder](const FString& Path)
	{
		folder = Path;
	});
	window->SetContent(
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get().CreatePathPicker(config)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Right)
		.Padding(8.f)
		[
			SNew(SButton)
			.Text(LOCTEXT("ImportHere", "Import Here"))
			.OnClicked_Lambda([&accepted, weakWindow]()
			{
				accepted = true;
				if (auto pinned = weakWindow.Pin())
					pinned->RequestDestroyWindow();
				return FReply::Handled();
			})
		]
	);
	FSlateApplication::Get().AddModalWindow(window, FSlateApplication::Get().FindWidgetWindow(AsShared()));

	if (!accepted)
		return FReply::Handled();

	TArray<UMounteaRecipeTemplate*> imported;
	FString error;

	if (!UMounteaAdvancedInventoryRecipeEditorStatics::ImportRecipesFromFilePath(files[0], folder, imported, error))
		ShowError(error);
	else
	{
		RefreshRecipes();
		if (!imported.IsEmpty())
			SelectRecipe(imported[0]);
		FMounteaEditorNotificationHelpers::ShowNotification(FString::Printf(TEXT("Imported %d recipes."), imported.Num()), EMounteaEditorNotificationType::Success);
	}
	return FReply::Handled();
}

FReply SMounteaCraftingRecipesEditor::ExportRecipes()
{
	TArray<UMounteaRecipeTemplate*> selected;
	for (const auto& item : Tree->GetSelectedItems())
	{
		if (item->Recipe.IsValid())
			selected.Add(item->Recipe.Get());
	}

	if (selected.IsEmpty())
	{
		ShowError(TEXT("Select at least one recipe to export."));
		return FReply::Handled();
	}

	IDesktopPlatform* desktop = FDesktopPlatformModule::Get();

	if (!desktop)
	{
		ShowError(TEXT("File dialogs are unavailable."));
		return FReply::Handled();
	}

	TArray<FString> files;

	const bool single = selected.Num() == 1;
	const FString extension = single ? TEXT("mntearecipe") : TEXT("mntearecipes");

	if (desktop->SaveFileDialog(FSlateApplication::Get().FindBestParentWindowHandleForDialogs(AsShared()), TEXT("Export Crafting Recipes"), FString(),
		TEXT("recipes.") + extension, single ? TEXT("Mountea Recipe (*.mntearecipe)|*.mntearecipe") : TEXT("Mountea Recipes (*.mntearecipes)|*.mntearecipes"), EFileDialogFlags::None, files) && !files.IsEmpty())
	{
		FString error;

		const FString file = FPaths::ChangeExtension(files[0], extension);

		if (!UMounteaAdvancedInventoryRecipeEditorStatics::ExportRecipesToFilePath(selected, file, error))
			ShowError(error);
		else
			FMounteaEditorNotificationHelpers::ShowNotification(TEXT("Recipes exported successfully."), EMounteaEditorNotificationType::Success);
	}
	return FReply::Handled();
}

FReply SMounteaCraftingRecipesEditor::ShowHelp()
{
	TSharedRef<SWindow> window = SNew(SWindow)
		.Title(LOCTEXT("HelpTitle", "Crafting Recipes Editor Tutorial"))
		.ClientSize(FVector2D(660, 380))
		.SupportsMinimize(false)
		.SupportsMaximize(false);
	window->SetContent(
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		.Padding(20.f)
		[
			SNew(STextBlock)
			.AutoWrapText(true)
			.Text(LOCTEXT("HelpText",
			"Use New Template to create a recipe, select its result item, quantity, crafting time and optional station tag. Add ingredients directly in Recipe Ingredients: each entry has an Ingredient Source and Required Quantity. Every ingredient is required.\n\nRecipes are grouped by their result item category. Search matches result names, station tags, categories, asset paths and recipe GUIDs. Select several recipes to edit common properties or export a bundle.\n\nSave Template saves the current recipe; Save All Templates saves modified recipes. Duplicate creates a draft with a new GUID. Saving preserves the GUID. Delete uses the standard asset deletion dialog. Existing asset edits support Undo and Redo.\n\nThe Allowed Recipes list in your crafting configuration controls which recipes are available at runtime. Import and export use .mntearecipe and .mntearecipes files."))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Right)
		.Padding(12.f)
		[
			SNew(SButton)
			.Text(LOCTEXT("Website", "Open Mountea Website"))
			.OnClicked_Lambda([]()
			{
				FPlatformProcess::LaunchURL(TEXT("https://mountea.tools"), nullptr, nullptr);
				return FReply::Handled();
			})
		]
	);
	FSlateApplication::Get().AddModalWindow(window, FSlateApplication::Get().FindWidgetWindow(AsShared()));
	return FReply::Handled();
}
#undef LOCTEXT_NAMESPACE
