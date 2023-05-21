// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInventoryHelpers.h"
#include "Interfaces/MounteaInventoryInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"
#include "MounteaInventoryEquipmentBPF.generated.h"

class IMounteaInventoryInterface;
/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryEquipmentBPF : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(CompactNodeTitle="Is Valid", DisplayName="Is Valid", NativeBreakFunc))
	static bool ItemRetrievalFilter_IsValid(const FItemRetrievalFilter& Filter)
	{
		return Filter.IsValid();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static FIntPoint GetInventoryDimensions()
	{
		if (const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>())
		{
			return Settings->BaseInventoryDimensions;
		}

		return FIntPoint(6, 8);
	}

	/**
	* Tries to retrieve the inventory interface of the specified Actor.
	* If Actor doesn't implement interface, then first found interface in Components will be returned.
	* If nothing is found, then nullptr is returned.
	* 
	* @param FromActor The actor from which to retrieve the inventory interface.
	* @return The inventory interface as a script interface of type IMounteaInventoryInterface.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TScriptInterface<IMounteaInventoryInterface> GetInventoryInterface(AActor* FromActor)
	{
		return GetInterfaceFrom<UMounteaInventoryInterface, TScriptInterface<IMounteaInventoryInterface>>(FromActor);
	}

	template<class Interface, class InterfaceClass>
	static InterfaceClass GetInterfaceFrom(AActor* From)
	{
		if (From == nullptr) return nullptr;

		if (From->Implements<Interface>()) return From;

		TArray<UActorComponent*> Components;
		From->GetComponents(Components);

		for (auto Itr : Components)
		{
			if (Itr && Itr->Implements<Interface>())
			{
				return Itr;
			}
		}
		return nullptr;
	}
};

