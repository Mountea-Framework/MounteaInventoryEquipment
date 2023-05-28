// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "MounteaInventoryHelpers.h"
#include "Definitions/MounteaInventoryItem.h"
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
		return GetThemeConfig()->InventoryBaseSize;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static FIntPoint GetInventorySlotSize()
	{
		return GetThemeConfig()->SlotBaseSize;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool UIDebug()
	{
		if (IsShipping())
		{
			return false;
		}
		
		if (const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>())
		{
			return Settings->bUIDebug;
		}

		return false;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static UMounteaInventoryThemeConfig* GetThemeConfig()
	{
		const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>();
		if (!Settings)
		{
			return NewObject<UMounteaInventoryThemeConfig>();
		}

		if (!Settings->ThemeConfig)
		{
			return NewObject<UMounteaInventoryThemeConfig>();
		}
		
		return Settings->ThemeConfig.LoadSynchronous();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool IsEditor()
	{
		
#if WITH_EDITOR
		return true;
#endif

		return false;
	}
	 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool IsShipping()
	{
		
#if UE_BUILD_SHIPPING
		return true;
#endif

		return false;
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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static int CalculateMaxAddQuantity(UMounteaInventoryItemBase* Item, UMounteaInventoryItemBase* OtherItem = nullptr, const int32 RequestedQuantity = 1);

	static bool AddItemQuantity(UMounteaInventoryItemBase* BaseItem, UMounteaInventoryItemBase* OtherItem = nullptr, const int32 RequestedQuantity = 1);
	
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

