// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryCategoryWidgetInterface.generated.h"

class IMounteaAdvancedInventoryUIInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryCategoryWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryCategoryWidgetInterface
{
	GENERATED_BODY()

public:
	
	/**
	 * 
	 * @param CategoryId 
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories")
	void SetInventoryCategoryKey(const FString& CategoryId);
	virtual void SetInventoryCategoryKey_Implementation(const FString& CategoryId) = 0;

	/**
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories")
	FString GetInventoryCategoryKey() const;
	virtual FString GetInventoryCategoryKey_Implementation() const = 0;

	/**
	 * 
	 * @param bIsActive 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories")
	void SetActiveState(const bool bIsActive);
	virtual void SetActiveState_Implementation(const bool bIsActive) = 0;
};
