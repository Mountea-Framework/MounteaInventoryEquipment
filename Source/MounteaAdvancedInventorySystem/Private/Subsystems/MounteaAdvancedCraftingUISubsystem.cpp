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


#include "Subsystems/MounteaAdvancedCraftingUISubsystem.h"

#include "Interfaces/Crafting/MounteaAdvancedCraftingParticipantUIInterface.h"
#include "Logs/MounteaAdvancedInventoryLog.h"

void UMounteaAdvancedCraftingUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UMounteaAdvancedCraftingUISubsystem::Deinitialize()
{
	UnregisterCraftingUIManager(CraftingUIManager.GetObject());

	Super::Deinitialize();
}

void UMounteaAdvancedCraftingUISubsystem::RegisterCraftingUIManager(UObject* NewCraftingUIManager)
{
	if (!NewCraftingUIManager)
	{
		LOG_WARNING(TEXT("[RegisterCraftingUIManager] Attempted to register null object"))
		return;
	}

	if (!NewCraftingUIManager->Implements<UMounteaAdvancedCraftingParticipantUIInterface>())
	{
		LOG_ERROR(TEXT("[RegisterCraftingUIManager] Object does not implement IMounteaAdvancedCraftingParticipantUIInterface"))
		return;
	}

	if (CraftingUIManager.GetObject())
		LOG_WARNING(TEXT("[RegisterCraftingUIManager] Overwriting existing manager reference"))

	CraftingUIManager.SetObject(NewCraftingUIManager);
	CraftingUIManager.SetInterface(Cast<IMounteaAdvancedCraftingParticipantUIInterface>(NewCraftingUIManager));
}

void UMounteaAdvancedCraftingUISubsystem::UnregisterCraftingUIManager(const UObject* DirtyCraftingUIManager)
{
	if (CraftingUIManager.GetObject() == DirtyCraftingUIManager)
	{
		CraftingUIManager.SetObject(nullptr);
		CraftingUIManager.SetInterface(nullptr);
	}
}

UObject* UMounteaAdvancedCraftingUISubsystem::GetCraftingUIManager() const
{
	return CraftingUIManager.GetObject();
}

TScriptInterface<IMounteaAdvancedCraftingParticipantUIInterface> UMounteaAdvancedCraftingUISubsystem::GetCraftingUIManager_Interface() const
{
	return CraftingUIManager;
}
