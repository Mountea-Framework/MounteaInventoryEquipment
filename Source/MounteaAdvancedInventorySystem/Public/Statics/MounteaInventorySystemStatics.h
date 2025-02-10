// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MounteaInventorySystemStatics.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventorySystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static bool CanExecuteCosmeticEvents(const UWorld* WorldContext);
};
