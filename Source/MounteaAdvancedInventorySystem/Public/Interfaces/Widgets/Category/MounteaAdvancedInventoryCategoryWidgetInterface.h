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
	 * @param OwningInventoryUI 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Items")
	void SetOwningInventoryUI(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI);
	virtual void SetOwningInventoryUI_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& OwningInventoryUI) = 0;
};
