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

#include "Statics/MounteaInventoryModalStatics.h"

#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/Widgets/Modal/MounteaAdvancedInventoryModalContentWidgetInterface.h"
#include "Interfaces/Widgets/Modal/MounteaAdvancedInventoryModalWidgetInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/MounteaAdvancedInventoryGlobalUIConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"

UUserWidget* UMounteaInventoryModalStatics::CreateModalWindowWidget(UObject* Context)
{
	if (!IsValid(Context))
		return nullptr;

	UMounteaAdvancedInventoryGlobalUIConfig* globalUIConfig = GetGlobalUIConfig();
	if (!IsValid(globalUIConfig))
		return nullptr;

	if (globalUIConfig->ModalWindowWidgetClass.IsNull())
		return nullptr;

	TSubclassOf<UUserWidget> widgetClass = globalUIConfig->ModalWindowWidgetClass.LoadSynchronous();
	if (!widgetClass || !widgetClass->ImplementsInterface(UMounteaAdvancedInventoryModalWidgetInterface::StaticClass()))
		return nullptr;

	UUserWidget* newWidget = CreateWidgetFromClass(Context, widgetClass);
	if (!IsValid(newWidget))
		return nullptr;

	return newWidget;
}

UUserWidget* UMounteaInventoryModalStatics::CreateModalContentWidget(UObject* Context, const FString& ModalType, const FString& Key, UObject* OptionalPayload)
{
	if (!IsValid(Context))
		return nullptr;

	UMounteaAdvancedInventoryGlobalUIConfig* globalUIConfig = GetGlobalUIConfig();
	if (!IsValid(globalUIConfig))
		return nullptr;

	if (ModalType.IsEmpty() || ModalType.Equals(TEXT("none"), ESearchCase::IgnoreCase))
		return nullptr;

	FDataTableRowHandle dataTableData = ResolveModalDataTableRow(globalUIConfig, Key);
	if (!IsValid(dataTableData.DataTable))
		return nullptr;

	const TSoftClassPtr<UUserWidget>* modalWidgetClass = globalUIConfig->Modals.Find(ModalType);
	if (!modalWidgetClass || modalWidgetClass->IsNull())
		return nullptr;

	TSubclassOf<UUserWidget> widgetClass = modalWidgetClass->LoadSynchronous();
	if (!widgetClass || !widgetClass->ImplementsInterface(UMounteaAdvancedInventoryModalContentWidgetInterface::StaticClass()))
		return nullptr;

	UUserWidget* newWidget = CreateWidgetFromClass(Context, widgetClass);
	if (!IsValid(newWidget))
		return nullptr;

	IMounteaAdvancedInventoryModalContentWidgetInterface::Execute_ConstructModalContent(newWidget, dataTableData, OptionalPayload, ModalType);

	return newWidget;
}

void UMounteaInventoryModalStatics::AddModalContentToModalWindow(
	UObject* Target,
	UUserWidget* ModalContentWidget)
{
	if (!IsValid(Target) || !IsValid(ModalContentWidget))
		return;

	if (Target->Implements<UMounteaAdvancedInventoryModalWidgetInterface>())
		IMounteaAdvancedInventoryModalWidgetInterface::Execute_AddModalContentToModalWindow(Target, ModalContentWidget);
}

FDataTableRowHandle UMounteaInventoryModalStatics::ResolveModalDataTableRow(
	const UMounteaAdvancedInventoryGlobalUIConfig* GlobalUIConfig,
	const FString& Key)
{
	FDataTableRowHandle dataTableData;
	if (!IsValid(GlobalUIConfig) || Key.IsEmpty() || Key.Equals(TEXT("none"), ESearchCase::IgnoreCase))
		return dataTableData;

	dataTableData.DataTable = FindModalDataTableForRow(GlobalUIConfig, Key);
	dataTableData.RowName = FName(*Key);
	return dataTableData;
}

UDataTable* UMounteaInventoryModalStatics::FindModalDataTableForRow(const UMounteaAdvancedInventoryGlobalUIConfig* GlobalUIConfig, const FString& Key)
{
	if (!IsValid(GlobalUIConfig) || Key.IsEmpty() || Key.Equals(TEXT("none"), ESearchCase::IgnoreCase))
		return nullptr;

	const FName rowName(*Key);
	for (const TSoftObjectPtr<UDataTable>& modalDataTable : GlobalUIConfig->ModalsData)
	{
		UDataTable* dataTable = modalDataTable.LoadSynchronous();
		if (!IsValid(dataTable))
			continue;

		if (dataTable->GetRowNames().Contains(rowName))
			return dataTable;
	}

	return nullptr;
}

UUserWidget* UMounteaInventoryModalStatics::CreateWidgetFromClass(UObject* Context, const TSubclassOf<UUserWidget> WidgetClass)
{
	if (!IsValid(Context) || !WidgetClass)
		return nullptr;

	if (APlayerController* owningPlayer = ResolveOwningPlayer(Context))
		return CreateWidget<UUserWidget>(owningPlayer, WidgetClass);

	if (UWorld* world = Context->GetWorld())
		return CreateWidget<UUserWidget>(world, WidgetClass);

	return nullptr;
}

APlayerController* UMounteaInventoryModalStatics::ResolveOwningPlayer(UObject* Context)
{
	if (!IsValid(Context))
		return nullptr;

	if (APlayerController* playerController = Cast<APlayerController>(Context))
		return playerController;

	if (const UUserWidget* userWidget = Cast<UUserWidget>(Context))
	{
		if (APlayerController* owningPlayer = userWidget->GetOwningPlayer())
			return owningPlayer;
	}

	return UGameplayStatics::GetPlayerController(Context, 0);
}

UMounteaAdvancedInventoryGlobalUIConfig* UMounteaInventoryModalStatics::GetGlobalUIConfig()
{
	const UMounteaAdvancedInventorySettings* settings = GetDefault<UMounteaAdvancedInventorySettings>();
	if (!IsValid(settings))
		return nullptr;

	return settings->GlobalUIConfig.LoadSynchronous();
}
