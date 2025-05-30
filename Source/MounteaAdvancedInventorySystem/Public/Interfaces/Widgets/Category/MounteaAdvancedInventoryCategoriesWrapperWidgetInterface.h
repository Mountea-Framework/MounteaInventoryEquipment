// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaAdvancedInventoryCategoriesWrapperWidgetInterface.generated.h"

class IMounteaAdvancedInventoryUIInterface;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaAdvancedInventoryCategoriesWrapperWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryCategoriesWrapperWidgetInterface
{
	GENERATED_BODY()

public:

	/**
	 * 
	 * @param ActiveCategoryId 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories")
	void SetActiveCategoryId(const FString& ActiveCategoryId);
	virtual void SetActiveCategoryId_Implementation(const FString& ActiveCategoryId) = 0;

	/**
	 * 
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Inventory|UI|Categories")
	FString GetActiveCategoryId() const;
	virtual FString GetActiveCategoryId_Implementation() const = 0;
};
