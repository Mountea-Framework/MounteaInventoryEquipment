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


#include "Subsystems/MounteaAdvancedInventoryUISubsystem.h"

#include "Interfaces/Inventory/MounteaAdvancedInventoryUIInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"

void UMounteaAdvancedInventoryUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UMounteaAdvancedInventoryUISubsystem::Deinitialize()
{
	UnregisterInventoryUIComponent(InventoryUIManager.GetObject());
	
	Super::Deinitialize();
}

void UMounteaAdvancedInventoryUISubsystem::RegisterInventoryUIManager(UObject* NewInventoryUIManager)
{
	if (!NewInventoryUIManager)
	{
		LOG_WARNING(TEXT("[RegisterInventoryUIManager] Attempted to register null object"))
		return;
	}

	if (!NewInventoryUIManager->Implements<UMounteaAdvancedInventoryUIInterface>())
	{
		LOG_ERROR(TEXT("[RegisterInventoryUIManager] Object does not implement IMounteaAdvancedInventoryUIInterface"))
		return;
	}

	if (InventoryUIManager.GetObject())
	{
		LOG_WARNING(TEXT("[RegisterInventoryUIManager] Overwriting existing manager reference"))
	}

	InventoryUIManager.SetObject(NewInventoryUIManager);
	InventoryUIManager.SetInterface(Cast<IMounteaAdvancedInventoryUIInterface>(NewInventoryUIManager));
}

void UMounteaAdvancedInventoryUISubsystem::UnregisterInventoryUIComponent(const UObject* DirtyInventoryUIManager)
{
	if (InventoryUIManager.GetObject() == DirtyInventoryUIManager)
	{
		InventoryUIManager.SetObject(nullptr);
		InventoryUIManager.SetInterface(nullptr);
	}
}

UObject* UMounteaAdvancedInventoryUISubsystem::GetInventoryUIManager() const
{
	return InventoryUIManager.GetObject();
}
