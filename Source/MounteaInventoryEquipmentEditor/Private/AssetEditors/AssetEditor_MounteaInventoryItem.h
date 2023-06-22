// Copyright Dominik Pavlicek 2023. All Rights Reserved.

#pragma once

class FAssetEditorToolbarMounteaInventoryItem;
class UMounteaInventoryItemBase;
class FGGAssetEditorToolbar;
class FAssetEditorToolbarMounteaDialogueGraph;

class MOUNTEAINVENTORYEQUIPMENTEDITOR_API FAssetEditor_MounteaInventoryItem : public FAssetEditorToolkit, public FNotifyHook, public FGCObject
{

public:
	void OnPackageSaved(const FString& String, UPackage* Package, FObjectPostSaveContext ObjectPostSaveContext);
	
	FAssetEditor_MounteaInventoryItem();
	virtual ~FAssetEditor_MounteaInventoryItem() override;
	
	void InitAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UMounteaInventoryItemBase* Item);
	

#pragma region AssetEditorToolkit

	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FString GetDocumentationLink() const override;

#pragma endregion

#pragma region Toolbar

	void UpdateToolbar();
	TSharedPtr<FAssetEditorToolbarMounteaInventoryItem> GetToolbarBuilder() { return ToolbarBuilder; }
	void RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager);

#pragma endregion

#pragma region SerializableObjectInterface

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

#pragma endregion

#pragma region FGCObject

	virtual FString GetReferencerName() const override;

#pragma endregion 
	
	
private:

	void BindCommands();
	void CreateCommandList();


#pragma region EditorCommands

	void RefreshData();
	bool CanRefreshData() const;

#pragma endregion

#pragma region Variables
	
private:

	UMounteaInventoryItemBase* EditingItem;

	//Toolbar
	TSharedPtr<FAssetEditorToolbarMounteaInventoryItem> ToolbarBuilder;

	/** Handle to the registered OnPackageSave delegate */
	FDelegateHandle OnPackageSavedDelegateHandle;

	/** The command list for this editor */
	TSharedPtr<FUICommandList> Commands;

#pragma endregion 
};
