// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "InputCore/Classes/InputCoreTypes.h"
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
		bool bFound;
		const auto Config = GetThemeConfig(UMounteaInventoryThemeConfig::StaticClass(), bFound);
		return bFound ? Config->InventoryBaseSize : FIntPoint(6, 10);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static FIntPoint GetInventorySlotSize()
	{
		bool bFound;
		const auto Config = GetThemeConfig(UMounteaInventoryThemeConfig::StaticClass(), bFound);
		return bFound ? Config->SlotBaseSize : FIntPoint(16, 16);
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
	static bool IsDragAllowed()
	{
		return GetSettings()->bDragDropAllowed;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaInventoryThemeConfig"), meta=(DeterminesOutputType = "ClassFilter"))
	static UMounteaInventoryThemeConfig* GetThemeConfig(const TSubclassOf<UMounteaInventoryThemeConfig> ClassFilter, bool& bResult)
	{
		if (ClassFilter == nullptr)
		{
			bResult = false;
			return nullptr;
		}

		bResult = true;
		const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>();
		if (!Settings)
		{
			return NewObject<UMounteaInventoryThemeConfig>();
		}

		if (Settings->ThemeConfig.IsNull())
		{
			return NewObject<UMounteaInventoryThemeConfig>(nullptr, ClassFilter);
		}

		auto const FoundTheme = Settings->ThemeConfig.LoadSynchronous();
		return FoundTheme->IsA(ClassFilter) ? FoundTheme : NewObject<UMounteaInventoryThemeConfig>(nullptr, ClassFilter);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaInventoryItemConfig"), meta=(DeterminesOutputType = "ClassFilter"))
	static UMounteaInventoryItemConfig* GetItemConfig(const UMounteaInventoryItemBase* Target, const TSubclassOf<UMounteaInventoryItemConfig> ClassFilter, bool& bResult)
	{
		if (ClassFilter == nullptr)
		{
			bResult = false;
			return nullptr;
		}
		
		if (Target == nullptr)
		{
			bResult = false;
			return nullptr;
		}
		
		return Target->GetItemConfig(ClassFilter, bResult);
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static const UMounteaInventoryEquipmentSettings* GetSettings()
	{
		return GetDefault<UMounteaInventoryEquipmentSettings>();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TSet<UMounteaInventoryItemCategory*> GetAllowedCategories()
	{
		TSet<UMounteaInventoryItemCategory*> ReturnValues;
		for (auto Itr : GetSettings()->InventoryCategories)
		{
			ReturnValues.Add(Itr.LoadSynchronous());
		}

		return ReturnValues;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TSet<UMounteaInventoryItemRarity*> GetAllowedRarities()
	{
		TSet<UMounteaInventoryItemRarity*> ReturnValues;
		for (auto Itr : GetSettings()->InventoryRarities)
		{
			ReturnValues.Add(Itr.LoadSynchronous());
		}

		return ReturnValues;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TArray<FKey> GetDragKeys()
	{
		return GetSettings()->DragKeys;
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

