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
#include "UObject/Interface.h"
#include "MounteaAdvancedEquipmentItemInterface.generated.h"

enum class EEquipmentItemState : uint8;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UMounteaAdvancedEquipmentItemInterface : public UInterface
{
	GENERATED_BODY()
};

class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedEquipmentItemInterface
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|EquipmentItem")
	EEquipmentItemState GetEquipmentItemState() const;
	virtual EEquipmentItemState GetEquipmentItemState_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|EquipmentItem")
	bool SetEquipmentItemState(const EEquipmentItemState NewEquipmentItemState);
	virtual bool SetEquipmentItemState_Implementation(const EEquipmentItemState NewEquipmentItemState) = 0;
};
