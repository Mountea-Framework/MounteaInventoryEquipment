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
#include "Definitions/MounteaInventoryBaseCommands.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Helpers/MounteaModalsPayload.h"
#include "Interfaces/Widgets/MounteaInventoryGenericWidgetInterface.h"
#include "Interfaces/Widgets/Modal/MounteaAdvancedInventoryModalContentWidgetInterface.h"
#include "Interfaces/Widgets/Modal/MounteaAdvancedInventoryModalWidgetInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/MounteaAdvancedInventoryGlobalUIConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"

#define MOUNTEA_BIND_MODAL_CONTENT_DELEGATE(Target, Binding, HandleGetter) \
	if (!IsValid(Target) || !(Binding).IsBound()) \
		return false; \
	IMounteaAdvancedInventoryModalContentWidgetInterface* nativeInterface = Cast<IMounteaAdvancedInventoryModalContentWidgetInterface>(Target); \
	if (!nativeInterface) \
		return false; \
	nativeInterface->HandleGetter().AddUnique(Binding); \
	return true

#define MOUNTEA_UNBIND_MODAL_CONTENT_DELEGATE(Target, Binding, HandleGetter) \
	if (!IsValid(Target) || !(Binding).IsBound()) \
		return false; \
	IMounteaAdvancedInventoryModalContentWidgetInterface* nativeInterface = Cast<IMounteaAdvancedInventoryModalContentWidgetInterface>(Target); \
	if (!nativeInterface) \
		return false; \
	nativeInterface->HandleGetter().Remove(Binding); \
	return true

#define MOUNTEA_BIND_MODAL_WINDOW_DELEGATE(Target, Binding, HandleGetter) \
	if (!IsValid(Target) || !(Binding).IsBound()) \
		return false; \
	IMounteaAdvancedInventoryModalWidgetInterface* nativeInterface = Cast<IMounteaAdvancedInventoryModalWidgetInterface>(Target); \
	if (!nativeInterface) \
		return false; \
	nativeInterface->HandleGetter().AddUnique(Binding); \
	return true

#define MOUNTEA_UNBIND_MODAL_WINDOW_DELEGATE(Target, Binding, HandleGetter) \
	if (!IsValid(Target) || !(Binding).IsBound()) \
		return false; \
	IMounteaAdvancedInventoryModalWidgetInterface* nativeInterface = Cast<IMounteaAdvancedInventoryModalWidgetInterface>(Target); \
	if (!nativeInterface) \
		return false; \
	nativeInterface->HandleGetter().Remove(Binding); \
	return true

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

	if (newWidget->Implements<UMounteaInventoryGenericWidgetInterface>())
		IMounteaInventoryGenericWidgetInterface::Execute_ProcessInventoryWidgetCommand(newWidget, InventoryUICommands::General::Create, nullptr);
	
	return newWidget;
}

UUserWidget* UMounteaInventoryModalStatics::CreateModalContentWidget(UObject* Context, UMounteaModalsPayload* Payload)
{
	if (!IsValid(Context) || !IsValid(Payload))
		return nullptr;

	UMounteaAdvancedInventoryGlobalUIConfig* globalUIConfig = GetGlobalUIConfig();
	if (!IsValid(globalUIConfig))
		return nullptr;

	const FString& modalType = Payload->ModalType;
	if (modalType.IsEmpty() || modalType.Equals(TEXT("none"), ESearchCase::IgnoreCase))
		return nullptr;

	const TSoftClassPtr<UUserWidget>* modalWidgetClass = globalUIConfig->Modals.Find(modalType);
	if (!modalWidgetClass || modalWidgetClass->IsNull())
		return nullptr;

	TSubclassOf<UUserWidget> widgetClass = modalWidgetClass->LoadSynchronous();
	if (!widgetClass || !widgetClass->ImplementsInterface(UMounteaAdvancedInventoryModalContentWidgetInterface::StaticClass()))
		return nullptr;

	UUserWidget* newWidget = CreateWidgetFromClass(Context, widgetClass);
	if (!IsValid(newWidget))
		return nullptr;

	IMounteaAdvancedInventoryModalContentWidgetInterface::Execute_ConstructModalContent(newWidget, Payload);

	return newWidget;
}

void UMounteaInventoryModalStatics::AddModalContentToModalWindow(UObject* Target, UUserWidget* ModalContentWidget, UMounteaModalsPayload* Payload)
{
	if (!IsValid(Target) || !IsValid(ModalContentWidget))
		return;

	if (Target->Implements<UMounteaAdvancedInventoryModalWidgetInterface>())
		IMounteaAdvancedInventoryModalWidgetInterface::Execute_AddModalContentToModalWindow(Target, ModalContentWidget, Payload);
}

bool UMounteaInventoryModalStatics::BindToOnModalContentAddedToModalWindow(
	UObject* Target,
	const FMounteaModalContentAddedToModalWindowBinding& Binding)
{
	MOUNTEA_BIND_MODAL_WINDOW_DELEGATE(Target, Binding, GetOnModalContentAddedToModalWindowHandle);
}

bool UMounteaInventoryModalStatics::UnbindFromOnModalContentAddedToModalWindow(
	UObject* Target,
	const FMounteaModalContentAddedToModalWindowBinding& Binding)
{
	MOUNTEA_UNBIND_MODAL_WINDOW_DELEGATE(Target, Binding, GetOnModalContentAddedToModalWindowHandle);
}

bool UMounteaInventoryModalStatics::BindToOnModalConfirmed(
	UObject* Target,
	const FMounteaModalConfirmedBinding& Binding)
{
	MOUNTEA_BIND_MODAL_WINDOW_DELEGATE(Target, Binding, GetOnModalConfirmedHandle);
}

bool UMounteaInventoryModalStatics::UnbindFromOnModalConfirmed(
	UObject* Target,
	const FMounteaModalConfirmedBinding& Binding)
{
	MOUNTEA_UNBIND_MODAL_WINDOW_DELEGATE(Target, Binding, GetOnModalConfirmedHandle);
}

bool UMounteaInventoryModalStatics::BindToOnModalCancelled(
	UObject* Target,
	const FMounteaModalCancelledBinding& Binding)
{
	MOUNTEA_BIND_MODAL_WINDOW_DELEGATE(Target, Binding, GetOnModalCancelledHandle);
}

bool UMounteaInventoryModalStatics::UnbindFromOnModalCancelled(
	UObject* Target,
	const FMounteaModalCancelledBinding& Binding)
{
	MOUNTEA_UNBIND_MODAL_WINDOW_DELEGATE(Target, Binding, GetOnModalCancelledHandle);
}

bool UMounteaInventoryModalStatics::BindToOnModalContentConfirmed(
	UObject* Target,
	const FMounteaModalContentConfirmedBinding& Binding)
{
	MOUNTEA_BIND_MODAL_CONTENT_DELEGATE(Target, Binding, GetOnModalContentConfirmedHandle);
}

bool UMounteaInventoryModalStatics::UnbindFromOnModalContentConfirmed(
	UObject* Target,
	const FMounteaModalContentConfirmedBinding& Binding)
{
	MOUNTEA_UNBIND_MODAL_CONTENT_DELEGATE(Target, Binding, GetOnModalContentConfirmedHandle);
}

bool UMounteaInventoryModalStatics::BindToOnModalContentCancelled(
	UObject* Target,
	const FMounteaModalContentCancelledBinding& Binding)
{
	MOUNTEA_BIND_MODAL_CONTENT_DELEGATE(Target, Binding, GetOnModalContentCancelledHandle);
}

bool UMounteaInventoryModalStatics::UnbindFromOnModalContentCancelled(
	UObject* Target,
	const FMounteaModalContentCancelledBinding& Binding)
{
	MOUNTEA_UNBIND_MODAL_CONTENT_DELEGATE(Target, Binding, GetOnModalContentCancelledHandle);
}

UMounteaModalsPayload* UMounteaInventoryModalStatics::ConstructModalPayload(UObject* Target, const FString& ModalType, const FString& Key, UObject* OptionalPayload)
{
	if (!IsValid(Target))
		return nullptr;

	const auto modalsConfig = GetGlobalUIConfig();
	if (!IsValid(modalsConfig))
		return nullptr;

	if (ModalType.IsEmpty() || ModalType.Equals(TEXT("none"), ESearchCase::IgnoreCase))
		return nullptr;

	if (!modalsConfig->Modals.Contains(ModalType))
		return nullptr;

	const FDataTableRowHandle dataTableData = ResolveModalDataTableRow(modalsConfig, Key);
	if (!IsValid(dataTableData.DataTable) || dataTableData.RowName.IsNone())
		return nullptr;

	FString contextString;
	const FMounteaModalsConfig* modalConfig = dataTableData.DataTable->FindRow<FMounteaModalsConfig>(dataTableData.RowName, contextString);
	if (!modalConfig)
		return nullptr;

	const auto modalsPayloadClass = modalsConfig->ModalPayloadClass.LoadSynchronous();
	if (!IsValid(modalsPayloadClass))
		return nullptr;

	const auto returnValue = NewObject<UMounteaModalsPayload>(Target, modalsPayloadClass);
	if (!IsValid(returnValue))
		return nullptr;

	returnValue->ModalConfig = *modalConfig;
	returnValue->ModalType = ModalType;
	returnValue->OptionalPayload = OptionalPayload;

	return returnValue;
}

FDataTableRowHandle UMounteaInventoryModalStatics::ResolveModalDataTableRow(const UMounteaAdvancedInventoryGlobalUIConfig* GlobalUIConfig, const FString& Key)
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
