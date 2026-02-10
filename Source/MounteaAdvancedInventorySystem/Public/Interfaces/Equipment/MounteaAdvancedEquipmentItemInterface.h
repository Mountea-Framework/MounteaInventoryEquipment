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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentItemStateChanged, const TScriptInterface<IMounteaAdvancedEquipmentItemInterface>&, EquipmentItem, const EEquipmentItemState, NewState);

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
	FName GetEquipmentItemPreferredSlot() const;
	virtual FName GetEquipmentItemPreferredSlot_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|EquipmentItem")
	FGameplayTag GetEquipmentPreferredSloTag() const;
	virtual FGameplayTag GetEquipmentPreferredSloTag_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|EquipmentItem")
	EEquipmentItemState GetEquipmentItemState() const;
	virtual EEquipmentItemState GetEquipmentItemState_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|EquipmentItem")
	bool SetEquipmentItemState(const EEquipmentItemState NewEquipmentItemState);
	virtual bool SetEquipmentItemState_Implementation(const EEquipmentItemState NewEquipmentItemState) = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|EquipmentItem")
	bool DoesAutoActive() const;
	virtual bool DoesAutoActive_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|EquipmentItem")
	bool SetAutoActive(const bool bValue);
	virtual bool SetAutoActive_Implementation(const bool bValue) = 0;	
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|EquipmentItem")
	bool DoesRequireActivationEvent() const;
	virtual bool DoesRequireActivationEvent_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|EquipmentItem")
	bool SetRequiresActivationEvent(const bool bValue);
	virtual bool SetRequiresActivationEvent_Implementation(const bool bValue) = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|EquipmentItem")
	UAnimationAsset* GetActivationAnimation() const;
	virtual UAnimationAsset* GetActivationAnimation_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|EquipmentItem")
	bool SetActivationAnimation(UAnimationAsset* NewActivateAnimation);
	virtual bool SetActivationAnimation_Implementation(UAnimationAsset* NewActivateAnimation) = 0;
	
	virtual FOnEquipmentItemStateChanged& GetOnEquipmentItemStateChangedHandle() = 0;
};
