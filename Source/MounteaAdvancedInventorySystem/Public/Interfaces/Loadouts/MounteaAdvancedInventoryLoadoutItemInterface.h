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
#include "MounteaAdvancedInventoryLoadoutItemInterface.generated.h"

class UMounteaInventoryItemTemplate;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UMounteaAdvancedInventoryLoadoutItemInterface : public UInterface
{
	GENERATED_BODY()
};

class MOUNTEAADVANCEDINVENTORYSYSTEM_API IMounteaAdvancedInventoryLoadoutItemInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Loadouts|Item")
	UMounteaInventoryItemTemplate* GetItemTemplate() const;
	virtual UMounteaInventoryItemTemplate* GetItemTemplate_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Loadouts|Item")
	int32 GetQuantity() const;
	virtual int32 GetQuantity_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Loadouts|Item")
	bool IsUsingRandomQuantity() const;
	virtual bool IsUsingRandomQuantity_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory & Equipment|Loadouts|Item")
	FVector2f GetRandomQuantityRange() const;
	virtual FVector2f GetRandomQuantityRange_Implementation() const = 0;
};
