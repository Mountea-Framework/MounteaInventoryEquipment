// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "InputCore/Classes/InputCoreTypes.h"
#include "MounteaInventoryHelpers.h"

#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaItemAdditionalData.h"
#include "Definitions/MounteaInventoryItemRarity.h"
#include "Definitions/MounteaInventoryItemCategory.h"

#include "Interfaces/MounteaInventoryInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"
#include "WBP/MounteaTransactionPayload.h"

#include "Helpers/FMounteaTemplatesLibrary.h"

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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "Object"), meta=(DeterminesOutputType = "ClassFilter"))
	static UObject* GetObjectByClass(UObject* Object, const TSubclassOf<UObject> ClassFilter, bool& bResult)
	{
		if (ClassFilter == nullptr)
		{
			bResult = false;
			return nullptr;
		}

		if (Object == nullptr)
		{
			bResult = false;
			return nullptr;
		}

		if (Object->IsA(ClassFilter))
		{
			bResult = true;
			return Object;
		}

		bResult = false;
		return nullptr;
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
			return NewObject<UMounteaInventoryThemeConfig>(GetTransientPackage(), ClassFilter);
		}

		auto const FoundTheme = Settings->ThemeConfig.LoadSynchronous();
		return FoundTheme->IsA(ClassFilter) ? FoundTheme : NewObject<UMounteaInventoryThemeConfig>(GetTransientPackage(), ClassFilter);
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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaItemAdditionalData"), meta=(DeterminesOutputType = "ClassFilter"))
	static UMounteaItemAdditionalData* GetItemAdditionalData(const UMounteaInventoryItemBase* Target, const TSubclassOf<UMounteaItemAdditionalData> ClassFilter, bool& bResult)
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
		
		return Target->GetItemAdditionalData(ClassFilter, bResult);
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
	static TSubclassOf<UMounteaTransactionPayload> GetTransactionPayloadClass()
	{
		const auto Settings = GetSettings();
		return Settings->DefaultTransactionPayloadClass.IsNull() ? Settings->DefaultTransactionPayloadClass.LoadSynchronous() : UMounteaTransactionPayload::StaticClass();
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TArray<FKey> GetDragKeys()
	{
		return GetSettings()->DragKeys;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TArray<FIntPoint> CalculateItemShadow(const FIntPoint& StartCoords, const FIntPoint& Area)
	{
		TSet<FIntPoint> Results;

		int32 X = StartCoords.X;
		int32 Y = StartCoords.Y;

		for (int i = 0; i < Area.X; i++)
		{
			for (int j = 0; j < Area.Y; j++)
			{
				Results.Add(FIntPoint(X, Y));

				Y++;
			}

			X++;
		}
		
		return Results.Array();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool IsValidShadow(const TArray<FIntPoint>&  Canvas, const TArray<FIntPoint>& Shadow, const FIntPoint& Area)
	{
		if (Shadow.Num() < FMath::Max(Area.X, Area.Y))
		{
			return false;
		}

		for (auto const Itr : Shadow)
		{
			if (!Canvas.Contains(Itr))
			{
				return false;
			}
		}

		return true;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool IsSafeSlot(const FIntPoint& StartCoords, const FIntPoint& Area)
	{
		CalculateItemShadow(StartCoords, Area);

		return false;
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
};

