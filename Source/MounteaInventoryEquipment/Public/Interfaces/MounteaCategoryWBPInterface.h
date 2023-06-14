// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MounteaCategoryWBPInterface.generated.h"

class UMounteaInventoryItemCategory;

DECLARE_DYNAMIC_DELEGATE_OneParam(FGenericCategoryEvent, const UMounteaInventoryItemCategory*, CategoryDefinition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCategoryEvent, const UMounteaInventoryItemCategory*, CategoryDefinition);

class UMounteaInventoryItemCategory;
// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UMounteaCategoryWBPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaCategoryWBPInterface
{
	GENERATED_BODY()

public:
			
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void ProcessWidgetCommand(const FString& Command);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void RequestCreateCategory(const UMounteaInventoryItemCategory* CategoryDefinition);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void RequestCategorySelected(const UMounteaInventoryItemCategory* SelectedCategory);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Mountea|Inventory")
	void RequestUpdateStyle(const UMounteaInventoryItemCategory* ActiveCategory);
};
