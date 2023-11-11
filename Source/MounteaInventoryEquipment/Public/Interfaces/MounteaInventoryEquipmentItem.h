// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Helpers/MounteaInventoryHelpers.h"
#include "UObject/Interface.h"
#include "MounteaInventoryEquipmentItem.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UMounteaInventoryEquipmentItem : public UInterface
{
	GENERATED_BODY()
};

class UMounteaInventoryItemConfig;
class IMounteaInventoryInterface;

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaInventoryEquipmentItem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void SetValidData() = 0;

	virtual void SetGuid(const FGuid& NewGuid) = 0;
	virtual FGuid GetGuid() const = 0;

	virtual FMounteaInventoryItemData GetItemData() const = 0;
	
	virtual TSubclassOf<UMounteaInventoryItemConfig> GetItemConfigClass() const = 0;
	virtual UMounteaInventoryItemConfig* GetItemConfig(const TSubclassOf<UMounteaInventoryItemConfig> ClassFilter, bool& bResult) const = 0;
};
