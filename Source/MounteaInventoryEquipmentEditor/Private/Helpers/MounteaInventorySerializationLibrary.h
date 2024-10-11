// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInventorySerializationLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENTEDITOR_API UMounteaInventorySerializationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static FString ToJson(const struct FMounteaInventoryItemRequiredData& ItemRequiredData);
	
	static FString ToJson(const struct FMounteaInventoryItemOptionalData& ItemOptionalData);

	static FString ToJson(const struct FMounteaInventoryItemsLibraryRow& ItemBlueprintRow);
	
	static FString ToJson(const class UMounteaInventoryItemBlueprint* ItemBlueprint);

	static FString ToJson(const class UMounteaInventoryItemsLibrary* ItemBlueprintsLibrary);
};
