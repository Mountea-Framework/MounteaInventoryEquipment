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

#pragma once

#include "CoreMinimal.h"
#include "EditorUndoClient.h"
#include "UObject/GCObject.h"
#include "Misc/NotifyHook.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"

class IDetailsView;
class SSearchBox;
class UMounteaRecipeTemplate;
struct FAssetData;

struct FMounteaRecipeTreeItem
{
	FString Group;
	TWeakObjectPtr<UMounteaRecipeTemplate> Recipe;
	TArray<TSharedPtr<FMounteaRecipeTreeItem>> Children;
};

/** Native recipe authoring uses the same toolbar and browser/details layout as item templates. */
class SMounteaCraftingRecipesEditor : public SCompoundWidget, public FNotifyHook, public FEditorUndoClient, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(SMounteaCraftingRecipesEditor)
	{
	}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);
	virtual ~SMounteaCraftingRecipesEditor() override;
	bool CanCloseEditor();
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override
	{
		return TEXT("SMounteaCraftingRecipesEditor");
	}

	virtual void NotifyPostChange(const FPropertyChangedEvent& Event, FProperty* Property) override;
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override
	{
		PostUndo(bSuccess);
	}

private:
	friend class FMounteaCraftingRecipesEditorLifecycleTest;
	friend class FMounteaInventoryTreeRowParityTest;
	friend class FMounteaCraftingRecipesRegisteredTabTest;
	using FTreeItem = TSharedPtr<FMounteaRecipeTreeItem>;
	TSharedRef<SWidget> CreateToolbar();
	TSharedRef<ITableRow> GenerateRow(FTreeItem Item, const TSharedRef<STableViewBase>& Owner);
	void GetRecipeChildren(FTreeItem Item, TArray<FTreeItem>& Children) const;
	void SelectionChanged(FTreeItem Item, ESelectInfo::Type Type);
	void RefreshRecipes();
	void SelectRecipe(UMounteaRecipeTemplate* Recipe);
	void OnAssetChanged(const FAssetData& Asset);
	void OnAssetRenamed(const FAssetData& Asset, const FString& OldPath);
	EActiveTimerReturnType DeferredRefresh(double Time, float Delta);
	void OnObjectChanged(UObject* Object, FPropertyChangedEvent& Event);
	bool ConfirmDiscardDraft();
	bool SaveRecipe(UMounteaRecipeTemplate* Recipe);
	bool HasDirtyRecipes() const;
	void InitializeDraft();
	FReply CreateRecipe();
	FReply SaveCurrent();
	FReply SaveAll();
	FReply CloseRecipe();
	FReply ImportRecipes();
	FReply ReimportAll();
	FReply ExportRecipes();
	FReply ShowHelp();
	FReply DuplicateRecipe(TWeakObjectPtr<UMounteaRecipeTemplate> Recipe);
	FReply DeleteRecipe(TWeakObjectPtr<UMounteaRecipeTemplate> Recipe);

	TSharedPtr<IDetailsView> Details;
	TSharedPtr<SSearchBox> Search;
	TSharedPtr<STreeView<FTreeItem>> Tree;
	TArray<FTreeItem> Roots;
	TArray<TObjectPtr<UMounteaRecipeTemplate>> Recipes;
	TObjectPtr<UMounteaRecipeTemplate> Draft = nullptr;
	TWeakObjectPtr<UMounteaRecipeTemplate> Current;
	FString SearchText;
	TSet<FString> CollapsedGroups;
	bool bDraftDirty = false;
	bool bRefreshing = false;
	bool bRefreshPending = false;
	FDelegateHandle AddedHandle, RemovedHandle, UpdatedHandle, RenamedHandle, ChangedHandle;
};
