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

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Editor/SMounteaCraftingRecipesEditor.h"
#include "Editor/SMounteaInventoryTemplateEditor.h"
#include "Editor/STemplateTreeItemRow.h"
#include "Editor/MounteaInventoryEditorLayout.h"
#include "Definitions/MounteaInventoryItemTemplate.h"
#include "Definitions/MounteaRecipeIngredient.h"
#include "Definitions/MounteaRecipeTemplate.h"
#include "GameplayTagsManager.h"
#include "Editor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Blueprint.h"
#include "Framework/Docking/TabManager.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Statics/MounteaAdvancedInventoryRecipeEditorStatics.h"
#include "UObject/Package.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SWindow.h"
#include "Framework/Application/SlateApplication.h"
#include "ScopedTransaction.h"
#include "Layout/Children.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/MounteaAdvancedInventoryEditorStyle.h"
#include "ImageUtils.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "UObject/GarbageCollection.h"
#include "UObject/UnrealType.h"

namespace
{
	void FindWidgets(const TSharedRef<SWidget>& Widget, const FName Type, TArray<TSharedRef<SWidget>>& Found)
	{
		if (Widget->GetType() == Type) Found.Add(Widget);
		FChildren* Children = Widget->GetChildren();
		for (int32 Index = 0; Children && Index < Children->Num(); ++Index)
			FindWidgets(Children->GetChildAt(Index), Type, Found);
	}
	TSharedPtr<SButton> FindButton(const TSharedRef<SWidget>& Root, const FString& Label)
	{
		TArray<TSharedRef<SWidget>> Buttons; FindWidgets(Root, TEXT("SButton"), Buttons);
		for (const auto& Widget : Buttons)
		{
			TArray<TSharedRef<SWidget>> Text; FindWidgets(Widget, TEXT("STextBlock"), Text);
			for (const auto& Block : Text)
				if (StaticCastSharedRef<STextBlock>(Block)->GetText().ToString() == Label) return StaticCastSharedRef<SButton>(Widget);
		}
		return nullptr;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInventorySharedToolbarTest,
	"Mountea.InventorySystemEditor.Recipes.SharedToolbarActions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMounteaInventorySharedToolbarTest::RunTest(const FString& Parameters)
{
	if (!FSlateApplication::IsInitialized()) { AddError(TEXT("Slate is required for editor toolbar tests.")); return false; }
	FMounteaInventoryEditorToolbarActions Actions;
	TArray<FString> Invocations;
	auto Bind = [&Invocations](const TCHAR* Name)
	{ return FOnClicked::CreateLambda([&Invocations, Name]() { Invocations.Add(Name); return FReply::Handled(); }); };
	Actions.Create = Bind(TEXT("New")); Actions.Save = Bind(TEXT("Save")); Actions.SaveAll = Bind(TEXT("SaveAll"));
	Actions.Import = Bind(TEXT("Import")); Actions.Export = Bind(TEXT("Export"));
	Actions.Close = Bind(TEXT("Close")); Actions.Help = Bind(TEXT("Help"));
	Actions.CanSave = true; Actions.CanSaveAll = true;
	const auto Toolbar = MounteaInventoryEditorLayout::MakeToolbar(Actions, SNullWidget::NullWidget);
	const TArray<FString> Labels = {TEXT("New Template"), TEXT("Save Template"), TEXT("Save All Templates"),
		TEXT("Import Template"), TEXT("Export Item"), TEXT("Close Template"), TEXT("Show Tutorial")};
	for (const auto& Label : Labels)
	{
		const auto Button = FindButton(Toolbar, Label);
		if (TestTrue(*FString::Printf(TEXT("Toolbar contains %s"), *Label), Button.IsValid())) Button->SimulateClick();
	}
	TestEqual(TEXT("Every shared toolbar route invokes its own callback"), FString::Join(Invocations, TEXT(",")), FString(TEXT("New,Save,SaveAll,Import,Export,Close,Help")));
	Actions.CanSave = false; Actions.CanSaveAll = false;
	const auto DisabledToolbar = MounteaInventoryEditorLayout::MakeToolbar(Actions, SNullWidget::NullWidget);
	TestFalse(TEXT("Save follows its enabled attribute"), FindButton(DisabledToolbar, TEXT("Save Template"))->IsEnabled());
	TestFalse(TEXT("Save All follows its enabled attribute"), FindButton(DisabledToolbar, TEXT("Save All Templates"))->IsEnabled());
	const auto Layout = MounteaInventoryEditorLayout::MakeLayout(Toolbar,
		MounteaInventoryEditorLayout::MakeBrowserDetails(SNullWidget::NullWidget, SNullWidget::NullWidget, 100.f, 200.f));
	TArray<TSharedRef<SWidget>> Splitters; FindWidgets(Layout, TEXT("SSplitter"), Splitters);
	TestEqual(TEXT("Shared layout uses one browser/details splitter"), Splitters.Num(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaCraftingRecipesEditorLifecycleTest,
	"Mountea.InventorySystemEditor.Recipes.EditorLifecycle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMounteaCraftingRecipesEditorLifecycleTest::RunTest(const FString& Parameters)
{
	if (!GEditor || !FSlateApplication::IsInitialized()) { AddError(TEXT("An initialized editor and Slate are required.")); return false; }
	TSharedPtr<SMounteaCraftingRecipesEditor> Editor = SNew(SMounteaCraftingRecipesEditor);
	const auto NewButton = FindButton(Editor.ToSharedRef(), TEXT("New Template"));
	if (!TestTrue(TEXT("Recipe editor provides the shared New Template button"), NewButton.IsValid())) return false;
	NewButton->SimulateClick();
	if (!TestNotNull(TEXT("New toolbar action creates a draft"), Editor->Draft.Get())) return false;
	TWeakObjectPtr<UMounteaRecipeTemplate> Original = Editor->Draft.Get();
	const FGuid OriginalGuid = Original->RecipeGuid;
	CollectGarbage(RF_NoFlags);
	TestTrue(TEXT("Recipe draft survives garbage collection while its editor is open"), Original.IsValid());
	const int32 OriginalQuantity = Original->QuantityPerCreation;
	{
		const FScopedTransaction Transaction(NSLOCTEXT("MounteaRecipeEditorTest", "EditDraft", "Edit test recipe quantity"));
		Original->Modify(); Original->QuantityPerCreation = OriginalQuantity + 3;
		FPropertyChangedEvent Event(FindFProperty<FProperty>(UMounteaRecipeTemplate::StaticClass(), GET_MEMBER_NAME_CHECKED(UMounteaRecipeTemplate, QuantityPerCreation)));
		Original->PostEditChangeProperty(Event);
	}
	TestTrue(TEXT("Editor undo succeeds for recipe edits"), GEditor->UndoTransaction());
	TestEqual(TEXT("Undo restores the recipe quantity"), Original->QuantityPerCreation, OriginalQuantity);
	TestEqual(TEXT("Undo preserves recipe identity"), Original->RecipeGuid, OriginalGuid);
	// Avoid save dialogs in automation while exercising duplicate and close lifecycle routes.
	Editor->bDraftDirty = false;
	Editor->DuplicateRecipe(Original);
	TestTrue(TEXT("Duplicating creates a distinct recipe GUID"), Editor->Draft->RecipeGuid != OriginalGuid);
	TestEqual(TEXT("Duplicating retains recipe data"), Editor->Draft->QuantityPerCreation, OriginalQuantity);
	Editor->bDraftDirty = false;
	const auto CloseButton = FindButton(Editor.ToSharedRef(), TEXT("Close Template"));
	if (TestTrue(TEXT("Recipe editor provides the shared Close Template button"), CloseButton.IsValid())) CloseButton->SimulateClick();
	TestNull(TEXT("Close releases the unsaved draft"), Editor->Draft.Get());
	Editor.Reset();
	// A fresh instance binds fresh registry/undo delegates after the first instance was destroyed.
	Editor = SNew(SMounteaCraftingRecipesEditor);
	TestNull(TEXT("Reopening starts with a blank details canvas"), Editor->Draft.Get());
	TestFalse(TEXT("Reopened editor has no stale selection"), Editor->Current.IsValid());
	Editor.Reset();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaInventoryTreeRowParityTest,
	"Mountea.InventorySystemEditor.Recipes.TreeRowParity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMounteaInventoryTreeRowParityTest::RunTest(const FString& Parameters)
{
	if (!GEditor || !FSlateApplication::IsInitialized()) { AddError(TEXT("An initialized editor and Slate are required.")); return false; }
	const auto ItemEditor = SNew(SMounteaInventoryTemplateEditor);
	const auto RecipeEditor = SNew(SMounteaCraftingRecipesEditor);
	UPackage* Package = CreatePackage(TEXT("/Temp/MounteaRecipeRowParity"));
	auto* Result = NewObject<UMounteaInventoryItemTemplate>(Package);
	Result->DisplayName = FText::FromString(TEXT("Iron Ingot"));
	Result->ItemCategory = TEXT("Materials");
	auto* Recipe = NewObject<UMounteaRecipeTemplate>(Package);
	Recipe->ResultItem = Result;
	Package->SetDirtyFlag(false);
	const auto ItemNode = FTemplateTreeItem::MakeTemplate(Result);
	const auto RecipeNode = MakeShared<FMounteaRecipeTreeItem>(); RecipeNode->Recipe = Recipe;
	const auto ItemRow = ItemEditor->GenerateTemplateTreeRow(ItemNode, ItemEditor->TemplateTreeView.ToSharedRef())->AsWidget();
	const auto RecipeRow = RecipeEditor->GenerateRow(RecipeNode, RecipeEditor->Tree.ToSharedRef())->AsWidget();
	TArray<TSharedRef<SWidget>> ItemText, RecipeText, Buttons, Images, Borders;
	FindWidgets(ItemRow, TEXT("STextBlock"), ItemText); FindWidgets(RecipeRow, TEXT("STextBlock"), RecipeText);
	if (!TestEqual(TEXT("Item row has name and asset path"), ItemText.Num(), 2) ||
		!TestEqual(TEXT("Recipe row has name and asset path"), RecipeText.Num(), 2)) return false;
	TestEqual(TEXT("Recipe name contains only the result display name"), StaticCastSharedRef<STextBlock>(RecipeText[0])->GetText().ToString(), FString(TEXT("Iron Ingot")));
	TestEqual(TEXT("Recipe second line uses the same package path as item rows"), StaticCastSharedRef<STextBlock>(RecipeText[1])->GetText().ToString(), Package->GetName());
	for (int32 Index = 0; Index < 2; ++Index)
		TestTrue(TEXT("Recipe and item text use identical fonts"), StaticCastSharedRef<STextBlock>(RecipeText[Index])->GetFont() == StaticCastSharedRef<STextBlock>(ItemText[Index])->GetFont());
	FindWidgets(RecipeRow, TEXT("SButton"), Buttons);
	TSharedPtr<SWidget> AssetContent;
	for (const auto& Button : Buttons)
	{
		TArray<TSharedRef<SWidget>> Text;
		FindWidgets(Button, TEXT("STextBlock"), Text);
		if (Text.Num() == 2) { AssetContent = Button; break; }
	}
	if (!TestTrue(TEXT("Recipe row contains its shared name/path selection button"), AssetContent.IsValid())) return false;
	// Exclude the table row's expander button from the asset's four buttons.
	Buttons.Reset(); FindWidgets(AssetContent.ToSharedRef(), TEXT("SButton"), Buttons);
	FindWidgets(AssetContent.ToSharedRef(), TEXT("SImage"), Images);
	if (!TestEqual(TEXT("One row selector and three action buttons"), Buttons.Num(), 4)) return false;
	TestFalse(TEXT("Row selection button does not take keyboard focus"), StaticCastSharedRef<SButton>(Buttons[0])->SupportsKeyboardFocus());
	if (TestEqual(TEXT("Three icon actions replace text buttons"), Images.Num(), 3))
	{
		for (int32 Index = 0; Index < 3; ++Index)
		{
			Images[Index]->SlatePrepass();
			TestTrue(TEXT("Action icons use the item row's 16px size"), Images[Index]->GetDesiredSize() == FVector2D(16, 16));
		}
	}
	FMounteaInventoryAssetRowPresentation SelectedPresentation;
	SelectedPresentation.IsSelected = true;
	const auto SelectedContent = MounteaInventoryTreeRows::MakeAssetContent(SelectedPresentation);
	SelectedContent->SlatePrepass();
	FindWidgets(SelectedContent, TEXT("SBorder"), Borders);
	TestTrue(TEXT("Selected recipe uses the shared rounded gold selection brush"), Borders.ContainsByPredicate([](const TSharedRef<SWidget>& Widget)
	{ return StaticCastSharedRef<SBorder>(Widget)->GetBorderImage() == FMounteaAdvancedInventoryEditorStyle::Get().GetBrush("MAISStyleSet.TemplateItem.Selected"); }));
	TestFalse(TEXT("Recipe table row follows item row keyboard focus behavior"), RecipeRow->SupportsKeyboardFocus());
	RecipeEditor->CreateRecipe(); RecipeEditor->Draft->ResultItem = Result;
	FGameplayTagContainer Tags; UGameplayTagsManager::Get().RequestAllGameplayTags(Tags, true);
	RecipeEditor->Draft->RequiredCraftingPlace = Tags.First();
	RecipeEditor->RefreshRecipes();
	TSharedPtr<FMounteaRecipeTreeItem> Group;
	for (const auto& Root : RecipeEditor->Roots)
		for (const auto& Child : Root->Children)
			if (Child->Recipe.Get() == RecipeEditor->Draft) Group = Root;
	if (TestTrue(TEXT("Draft remains in its result category even with a station tag"), Group.IsValid()))
	{
		TestEqual(TEXT("Grouping uses only ResultItem.ItemCategory"), Group->Group, Result->ItemCategory);
		TArray<TSharedRef<SWidget>> HeaderText;
		FindWidgets(RecipeEditor->GenerateRow(Group, RecipeEditor->Tree.ToSharedRef())->AsWidget(), TEXT("STextBlock"), HeaderText);
		if (TestEqual(TEXT("Category header includes its count"), HeaderText.Num(), 2))
		{
			TestEqual(TEXT("Category casing matches item headers"), StaticCastSharedRef<STextBlock>(HeaderText[0])->GetText().ToString(), FString(TEXT("MATERIALS")));
			TestTrue(TEXT("Category font matches item headers"), StaticCastSharedRef<STextBlock>(HeaderText[0])->GetFont() == FCoreStyle::GetDefaultFontStyle("Bold", 12));
			TestEqual(TEXT("Category count reflects its rows"), StaticCastSharedRef<STextBlock>(HeaderText[1])->GetText().ToString(), FText::AsNumber(Group->Children.Num()).ToString());
		}
	}
	// Exercise the shared icon callbacks without opening browser/deletion dialogs.
	FMounteaInventoryAssetRowPresentation Actions;
	FString Invoked;
	Actions.Locate = FOnClicked::CreateLambda([&Invoked]() { Invoked += TEXT("L"); return FReply::Handled(); });
	Actions.Duplicate = FOnClicked::CreateLambda([&Invoked]() { Invoked += TEXT("D"); return FReply::Handled(); });
	Actions.Delete = FOnClicked::CreateLambda([&Invoked]() { Invoked += TEXT("X"); return FReply::Handled(); });
	Buttons.Reset(); FindWidgets(MounteaInventoryTreeRows::MakeAssetContent(Actions), TEXT("SButton"), Buttons);
	for (int32 Index = 1; Index < Buttons.Num(); ++Index) StaticCastSharedRef<SButton>(Buttons[Index])->SimulateClick();
	TestEqual(TEXT("Each icon dispatches its corresponding action"), Invoked, FString(TEXT("LDX")));
	RecipeEditor->bDraftDirty = false; RecipeEditor->CloseRecipe();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaCraftingRecipesRegisteredTabTest,
	"Mountea.InventorySystemEditor.Recipes.RegisteredTab",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMounteaCraftingRecipesRegisteredTabTest::RunTest(const FString& Parameters)
{
	if (!GEditor || !FSlateApplication::IsInitialized()) { AddError(TEXT("An initialized editor and Slate are required.")); return false; }
	// Match the actual module registration rather than constructing the content directly.
	const auto Tab = FGlobalTabmanager::Get()->TryInvokeTab(FName(TEXT("MounteaCraftingRecipesEditor")));
	if (!TestTrue(TEXT("The module's registered recipe tab opens"), Tab.IsValid())) return false;
	TestEqual(TEXT("Registered tab hosts the native crafting recipe editor"),
		Tab->GetContent()->GetType(), FName(TEXT("SMounteaCraftingRecipesEditor")));
	if (Tab->GetContent()->GetType() == FName(TEXT("SMounteaCraftingRecipesEditor")))
	{
		const auto RecipeEditor = StaticCastSharedRef<SMounteaCraftingRecipesEditor>(Tab->GetContent());
		for (const auto& Root : RecipeEditor->Roots)
		{
			if (Root->Children.IsEmpty()) continue;
			RecipeEditor->SelectRecipe(Root->Children[0]->Recipe.Get());
			break;
		}
		// Capture native Slate rendering for visual review in runs with a real RHI.
		const auto ParentWindow = Tab->GetParentWindow();
		const FVector2D PreviousSize = ParentWindow.IsValid() ? FVector2D(ParentWindow->GetClientSizeInScreen()) : FVector2D::ZeroVector;
		if (ParentWindow.IsValid()) ParentWindow->Resize(FVector2D(1280, 800));
		Tab->ActivateInParent(ETabActivationCause::SetDirectly);
		Tab->GetContent()->SlatePrepass();
		FSlateApplication::Get().Tick();
		if (const auto Window = Tab->GetParentWindow()) FSlateApplication::Get().ForceRedrawWindow(Window.ToSharedRef());
		TArray<FColor> Pixels; FIntVector Size;
		if (FSlateApplication::Get().TakeScreenshot(Tab->GetContent(), Pixels, Size) && Size.X > 0 && Size.Y > 0)
		{
			TArray64<uint8> Png;
			FImageUtils::PNGCompressImageArray(Size.X, Size.Y, Pixels, Png);
			const FString Folder = FPaths::ProjectSavedDir() / TEXT("RecipeSimplification");
			IFileManager::Get().MakeDirectory(*Folder, true);
			TestTrue(TEXT("Native recipe editor screenshot is saved"), FFileHelper::SaveArrayToFile(Png, *(Folder / TEXT("RecipesEditor.png"))));
		}
		else AddInfo(TEXT("Native recipe editor screenshot unavailable in this rendering mode."));
		if (ParentWindow.IsValid()) ParentWindow->Resize(PreviousSize);
	}
	TestTrue(TEXT("A clean recipe tab closes through its registered close handler"), Tab->RequestCloseTab());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaBundledCraftingRecipesTest,
	"Mountea.InventorySystemEditor.Recipes.BundledAssets",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMounteaBundledCraftingRecipesTest::RunTest(const FString& Parameters)
{
	auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
	const FString Folder = TEXT("/MounteaAdvancedInventorySystem/Data/Recipes");
	Registry.ScanPathsSynchronous({Folder}, true);
	TArray<FAssetData> Assets;
	Registry.GetAssetsByPath(*Folder, Assets, true);
	TSet<FGuid> Guids;
	int32 RecipeCount = 0;
	for (const auto& Asset : Assets)
	{
		auto* Recipe = Cast<UMounteaRecipeTemplate>(Asset.GetAsset());
		if (!Recipe) continue;
		++RecipeCount;
		const FString Path = Recipe->GetPathName();
		FString Error;
		TestTrue(*FString::Printf(TEXT("Bundled recipe %s validates: %s"), *Path, *Error),
			UMounteaAdvancedInventoryRecipeEditorStatics::ValidateRecipeTemplate(Recipe, Error));
		if (!Error.IsEmpty()) AddError(Path + TEXT(": ") + Error);
		TestTrue(*FString::Printf(TEXT("%s has required flat ingredients"), *Path), !Recipe->RecipeIngredients.IsEmpty());
		TestTrue(*FString::Printf(TEXT("%s has a valid recipe GUID"), *Path), Recipe->RecipeGuid.IsValid());
		TestFalse(*FString::Printf(TEXT("%s has a unique recipe GUID"), *Path), Guids.Contains(Recipe->RecipeGuid));
		Guids.Add(Recipe->RecipeGuid);
	}
	TestEqual(TEXT("All six bundled recipes, including the separate blade alternative, load"), RecipeCount, 6);
	const FArrayProperty* Ingredients = FindFProperty<FArrayProperty>(UMounteaRecipeTemplate::StaticClass(), TEXT("RecipeIngredients"));
	const FObjectProperty* IngredientObject = Ingredients ? CastField<FObjectProperty>(Ingredients->Inner) : nullptr;
	TestTrue(TEXT("RecipeIngredients is a flat array of ingredient objects"),
		IngredientObject && IngredientObject->PropertyClass == UMounteaRecipeIngredient::StaticClass());
	TestTrue(TEXT("Ingredient objects remain instanced recipe subobjects"),
		IngredientObject && IngredientObject->HasAnyPropertyFlags(CPF_InstancedReference));
	TestNull(TEXT("The old ingredient option UObject array is absent"),
		FindFProperty<FProperty>(UMounteaRecipeTemplate::StaticClass(), TEXT("RecipeIngredientOptions")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaPluginBlueprintCompileAuditTest,
	"Mountea.InventorySystemEditor.Recipes.PluginBlueprintCompileAudit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMounteaPluginBlueprintCompileAuditTest::RunTest(const FString& Parameters)
{
	auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
	Registry.ScanPathsSynchronous({TEXT("/MounteaAdvancedInventorySystem")}, true);
	FARFilter Filter;
	Filter.PackagePaths.Add(TEXT("/MounteaAdvancedInventorySystem"));
	Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
	Filter.bRecursivePaths = true;
	Filter.bRecursiveClasses = true;
	TArray<FAssetData> Assets;
	Registry.GetAssets(Filter, Assets);
	if (!TestTrue(TEXT("Plugin Blueprint callers are present for the compile audit"), !Assets.IsEmpty())) return false;
	int32 Compiled = 0;
	for (const auto& Asset : Assets)
	{
		auto* Blueprint = Cast<UBlueprint>(Asset.GetAsset());
		if (!TestNotNull(*FString::Printf(TEXT("Load Blueprint %s"), *Asset.GetObjectPathString()), Blueprint)) continue;
		FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
		FKismetEditorUtilities::CompileBlueprint(Blueprint);
		TestTrue(*FString::Printf(TEXT("Blueprint compiles after refreshing native crafting callers: %s"), *Blueprint->GetPathName()),
			Blueprint->Status != BS_Error);
		++Compiled;
	}
	// Audit only: compilation may dirty in-memory graphs, but never saves project assets.
	AddInfo(FString::Printf(TEXT("Refreshed and compiled %d plugin Blueprints without saving assets."), Compiled));
	return true;
}
#endif
