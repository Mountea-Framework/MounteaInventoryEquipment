// Copyright Dominik Pavlicek 2023. All Rights Reserved.

#include "AssetEditor_MounteaInventoryItem.h"

#include "FAssetEditorToolbarMounteaInventoryItem.h"
#include "GraphEditorActions.h"
#include "Definitions/MounteaInventoryItem.h"
#include "EditorCommands/FMounteaInventoryEquipmentEditorCommands.h"
#include "Framework/Commands/GenericCommands.h"

#include "UObject/ObjectSaveContext.h"

#define LOCTEXT_NAMESPACE "AssetEditorMounteaDialogueGraph"


void FAssetEditor_MounteaInventoryItem::OnPackageSaved(const FString& String, UPackage* Package, FObjectPostSaveContext ObjectPostSaveContext)
{
	
}

FAssetEditor_MounteaInventoryItem::FAssetEditor_MounteaInventoryItem()
{
	EditingItem = nullptr;
	OnPackageSavedDelegateHandle = UPackage::PackageSavedWithContextEvent.AddRaw(this, &FAssetEditor_MounteaInventoryItem::OnPackageSaved);
}

FAssetEditor_MounteaInventoryItem::~FAssetEditor_MounteaInventoryItem()
{
	EditingItem = nullptr;
	UPackage::PackageSavedWithContextEvent.Remove(OnPackageSavedDelegateHandle);

	FGenericCommands::Unregister();
	FGraphEditorCommands::Unregister();
	FMounteaInventoryEquipmentEditorCommands::Unregister();

	ToolbarBuilder.Reset();
}

void FAssetEditor_MounteaInventoryItem::InitAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UMounteaInventoryItemBase* Item)
{
	EditingItem = Item;
	
	FGenericCommands::Register();
	FGraphEditorCommands::Register();
	FMounteaInventoryEquipmentEditorCommands::Register();

	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShareable(new FAssetEditorToolbarMounteaInventoryItem(SharedThis(this)));
	}

	BindCommands();

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	ToolbarBuilder->AddMounteaItemToolbar(ToolbarExtender);
	
	RegenerateMenusAndToolbars();
}

FName FAssetEditor_MounteaInventoryItem::GetToolkitFName() const
{
	return FName("FMounteaItemEditor");
	return FAssetEditorToolkit::GetToolkitFName();
}

FText FAssetEditor_MounteaInventoryItem::GetBaseToolkitName() const
{
	return LOCTEXT("MounteaMounteaItemEditorAppLabel", "Mountea Item Editor");
	return FAssetEditorToolkit::GetBaseToolkitName();
}

FText FAssetEditor_MounteaInventoryItem::GetToolkitName() const
{
	return FAssetEditorToolkit::GetToolkitName();
}

FText FAssetEditor_MounteaInventoryItem::GetToolkitToolTipText() const
{
	return FAssetEditorToolkit::GetToolkitToolTipText();
}

FLinearColor FAssetEditor_MounteaInventoryItem::GetWorldCentricTabColorScale() const
{
	return FLinearColor::Gray;
	return FAssetEditorToolkit::GetWorldCentricTabColorScale();
}

FString FAssetEditor_MounteaInventoryItem::GetWorldCentricTabPrefix() const
{
	return TEXT("MounteaItemEditor");
	return FAssetEditorToolkit::GetWorldCentricTabPrefix();
}

FString FAssetEditor_MounteaInventoryItem::GetDocumentationLink() const
{
	return TEXT("https://github.com/Mountea-Framework/MounteaInventoryEquipment/wiki");
}

void FAssetEditor_MounteaInventoryItem::UpdateToolbar()
{
	// TODO
}

void FAssetEditor_MounteaInventoryItem::RegisterToolbarTab(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

void FAssetEditor_MounteaInventoryItem::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditingItem);
}

FString FAssetEditor_MounteaInventoryItem::GetReferencerName() const
{
	return TEXT("FAssetEditor_MounteaInventoryItem");
}

void FAssetEditor_MounteaInventoryItem::BindCommands()
{
	ToolkitCommands->MapAction
	(
		FMounteaInventoryEquipmentEditorCommands::Get().RefreshData,
		FExecuteAction::CreateSP(this, &FAssetEditor_MounteaInventoryItem::RefreshData),
		FCanExecuteAction::CreateSP(this, &FAssetEditor_MounteaInventoryItem::CanRefreshData)
	);
}


void FAssetEditor_MounteaInventoryItem::CreateCommandList()
{
	if (Commands.IsValid())
	{
		return;
	}

	Commands = MakeShareable(new FUICommandList);

	Commands->MapAction(FMounteaInventoryEquipmentEditorCommands::Get().RefreshData,
		FExecuteAction::CreateRaw(this, &FAssetEditor_MounteaInventoryItem::RefreshData),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_MounteaInventoryItem::CanRefreshData));
}


void FAssetEditor_MounteaInventoryItem::RefreshData()
{
	check(EditingItem != nullptr);

	const FScopedTransaction Transaction(LOCTEXT("MounteaDialogueGraphEditorAutoArrange", "Mountea Dialogue Graph Editor: Auto Arrange all Nodes"));

	EditingItem->Modify(true);

	EditingItem->SetValidData();
}

bool FAssetEditor_MounteaInventoryItem::CanRefreshData() const
{
	return true;
}

#undef LOCTEXT_NAMESPACE

