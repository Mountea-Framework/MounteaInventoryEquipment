// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "Editor.h"
#include "InputCore/Classes/InputCoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "MounteaInventoryHelpers.h"
#include "Components/MounteaInventoryComponent.h"

#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaItemAdditionalData.h"
#include "Definitions/MounteaInventoryItemRarity.h"
#include "Definitions/MounteaInventoryItemCategory.h"

#include "Settings/MounteaInventoryEquipmentSettings.h"

#include "WBP/MounteaTransactionPayload.h"
#include "WBP/MounteaBaseUserWidget.h"
#include "Helpers/FMounteaTemplatesLibrary.h"

#include "Interfaces/MounteaInventoryInterface.h"
#include "Interfaces/MounteaInventorySlotWBPInterface.h"
#include "Interfaces/MounteaInventoryItemWBPInterface.h"
#include "Setup/MounteaInventoryConfig.h"


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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(CompactNodeTitle="Is Valid", DisplayName="Is Valid", NativeBreakFunc))
	static bool ItemRetrievalFilter_IsValid(const FItemRetrievalFilter& Filter)
	{
		return Filter.IsValid();
	}

#pragma region Converter

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc, CompactNodeTitle="Array Valid"))
	static bool IsArrayValid(const TArray<UObject*>& Array)
	{
		return Array.Num() > 0;
	}

#pragma endregion 
	
#pragma region HelpersFunctions

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "Interface"), meta=(DeterminesOutputType = "InterfaceFilter"))
	static UActorComponent* GetSingleComponentByInterface(const AActor* Target, TSubclassOf<UInterface> InterfaceFilter)
	{
		if (Target == nullptr) return nullptr;

		TArray<UActorComponent*> TempComps = Target->GetComponentsByInterface(InterfaceFilter);

		if (TempComps.IsEmpty()) return nullptr;

		return TempComps[0];
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc, Keywords="valid,safe,null,check"), DisplayName="Is Valid (Inventory Ref)")
	static bool IsInventoryValid(const TScriptInterface<IMounteaInventoryInterface>& InventoryInterface)
	{
		return InventoryInterface.GetObject() != nullptr;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc, Keywords="find,all"))
	static TArray<UMounteaInventoryItemAction*> GetPurifiedItemActions(const UMounteaInventoryItemBase* Target)
	{
		if (!Target) return TArray<UMounteaInventoryItemAction*>();
		if (!Target->GetItemData().ItemCategory) return Target->GetItemActions();

		TArray<UMounteaInventoryItemAction*> ReturnValues = Target->GetItemData().ItemCategory->GetCategoryActions();

		for (auto const& Itr : Target->GetItemActions())
		{
			if (Itr && !ReturnValues.Contains(Itr))
			{
				ReturnValues.Add(Itr);
			}
		}
		
		return ReturnValues;
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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	static TSubclassOf<UMounteaInventoryThemeConfig> GetThemeConfigClass()
	{
		if (GetSettings()->ThemeConfig) return GetSettings()->ThemeConfig->StaticClass();

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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TSubclassOf<UMounteaInventoryConfig> GetItemInventoryConfigClass(const TScriptInterface<IMounteaInventoryInterface> Target, const TSubclassOf<UMounteaInventoryConfig> ClassFilter, bool& bResult)
	{
		if (!Target) return nullptr;

		return Target->Execute_GetInventoryConfigClass(Target.GetObject());
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TSubclassOf<UMounteaInventoryItemConfig> GetItemItemConfigClass(const UMounteaInventoryItemBase* Target)
	{
		if (!Target) return nullptr;

		return Target->GetItemConfigClass();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaInventoryConfig"), meta=(DeterminesOutputType = "ClassFilter"))
	static UMounteaInventoryConfig* GetInventoryConfig(const TScriptInterface<IMounteaInventoryInterface> Target, const TSubclassOf<UMounteaInventoryConfig> ClassFilter, bool& bResult)
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
		
		return Target->Execute_GetInventoryConfig(Target.GetObject(), ClassFilter, bResult);
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

#pragma endregion 
	
#pragma region EditorFunctions
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool IsEditor()
	{
#if WITH_EDITOR
		return true;
#endif

		return false;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool IsEditorNoPlay()
	{
		// This is to ensure we are not throwing InvalidWorld errors in Editor with no Gameplay.
		bool bIsEditorCall = false;
#if WITH_EDITOR
		if (GEditor != nullptr)
		{
			bIsEditorCall = !GEditor->GetPlayInEditorSessionInfo().IsSet();
		}
#endif

		return bIsEditorCall;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool IsShipping()
	{
		
#if UE_BUILD_SHIPPING
		return true;
#endif

		return false;
	}
	
#pragma endregion 

#pragma region SettingsFunctions

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static const UMounteaInventoryEquipmentSettings* GetSettings()
	{
		return GetDefault<UMounteaInventoryEquipmentSettings>();
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static UMounteaInventoryItemsTable* GetDefaultItemsTable()
	{
		return GetSettings()->DefaultInventoryItemDefinitionsTable.LoadSynchronous();
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
	static TArray<FKey> GetActionRequestKeys()
	{
		return GetSettings()->ActionRequestKeys;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TSoftClassPtr<UMounteaInventoryItemConfig> GetDefaultItemConfigClass()
	{
		return GetSettings()->DefaultItemConfigClass;
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
	
#pragma endregion 

#pragma region ThemeFunctions

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
	
#pragma endregion 
	
#pragma region QuantityFunctions

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static int CalculateMaxSubtractQuantity(UMounteaInventoryItemBase* Item, UMounteaInventoryItemBase* OtherItem = nullptr, const int32 RequestedQuantity = 1);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static int CalculateMaxAddQuantity(UMounteaInventoryItemBase* Item, UMounteaInventoryItemBase* OtherItem = nullptr, const int32 RequestedQuantity = 1);

	static bool AddItemQuantity(UMounteaInventoryItemBase* BaseItem, UMounteaInventoryItemBase* OtherItem = nullptr, const int32 RequestedQuantity = 1);

	static int32 GetValidFiltersCount(const FItemRetrievalFilter& Filter)
	{
		int32 Result = 0;

		if (Filter.bSearchByClass) Result++;
		if (Filter.bSearchByItem) Result++;
		if (Filter.bSearchByTag) Result++;
		if (Filter.bSearchByGUID) Result++;

		return Result;
	}

#pragma endregion 
	
#pragma region ItemFunctions

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc, Keywords="has,have,contain"))
	static bool DoesHaveTag(const UMounteaInventoryItemBase* Item, const FGameplayTag Tag)
	{
		if (Item == nullptr) return false;

		return Item->GetTags().HasTag(Tag);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory",meta=(NativeBreakFunc, Keywords="has,have,contain"))
	static bool DoesHaveAnyTag(const UMounteaInventoryItemBase* Item, const FGameplayTagContainer Tags)
	{
		if (Item == nullptr) return false;

		return Item->GetTags().HasAny(Tags);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc, Keywords="has,have,contain"))
	static bool DoesHaveAllTags(const UMounteaInventoryItemBase* Item, const FGameplayTagContainer Tags)
	{
		if (Item == nullptr) return false;

		return Item->GetTags().HasAll(Tags);
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
			Y = StartCoords.Y;
		}
		
		return Results.Array();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool IsValidShadow(const TArray<FIntPoint>&  SlotsCoords, const TArray<TScriptInterface<IMounteaInventorySlotWBPInterface>>& SlotsRefs, const TArray<FIntPoint>& Shadow, const TScriptInterface<IMounteaInventoryItemWBPInterface>& MovedItem)
	{
		if (MovedItem.GetObject() == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("[IsValidShadow - 00] Trying to move nothing!"))
			return false;
		}

		const FIntPoint Area = MovedItem->Execute_GetItemSize(MovedItem.GetObject());

		
		if (Shadow.Num() < FMath::Max(Area.X, Area.Y))
		{
			UE_LOG(LogTemp, Warning, TEXT("[IsValidShadow - 01] Trying to move outside Inventory grid!"))
			return false;
		}

		for (const auto& Itr : Shadow)
		{
			if (!SlotsCoords.Contains(Itr))
			{
				UE_LOG(LogTemp, Warning, TEXT("[IsValidShadow - 02] Shadow coords outsied Inventoyr grid!!"))
				return false;
			}
		}

		TMap<FIntPoint, TScriptInterface<IMounteaInventorySlotWBPInterface>> Slots;

		//Collect slots under shadow
		for (int i = 0; i < Shadow.Num(); i++)
		{
			const FIntPoint Coords = Shadow[i];
			
			if (!SlotsCoords.Contains(Coords))
			{
				UE_LOG(LogTemp, Warning, TEXT("[IsValidShadow - 03] Shadow coords outsied Inventoyr grid!!"))
				return false;
			}

			if (!SlotsCoords.IsValidIndex(SlotsCoords.Find(Coords)))
			{
				UE_LOG(LogTemp, Warning, TEXT("[IsValidShadow - 04] Shadow coords outsied Inventoyr grid!!"))
				return false;
			}
			
			const int CoordsIndex = SlotsCoords.Find(Coords);
			
			TScriptInterface<IMounteaInventorySlotWBPInterface> Slot = SlotsRefs.IsValidIndex(CoordsIndex) ? SlotsRefs[CoordsIndex] : nullptr;

			Slots.Add(Coords, Slot);
		}

		// Validate slot under shadow
		for (const auto& Itr : Shadow)
		{
			if (!SlotsCoords.Contains(Itr))
			{
				UE_LOG(LogTemp, Warning, TEXT("[IsValidShadow - 05] Shadow coords outsied Inventoyr grid!!"))
				return false;
			}

			const TScriptInterface<IMounteaInventorySlotWBPInterface>* Slot = Slots.Find(Itr);
			if (!Slot->GetInterface()->Execute_IsSlotEmpty(Slot->GetObject()) )
			{
				const TScriptInterface<IMounteaInventoryItemWBPInterface> OccupyingItem = Slot->GetInterface()->Execute_GetOccupyingItem(Slot->GetObject());
				if (OccupyingItem != MovedItem)
				{
					UE_LOG(LogTemp, Warning, TEXT("[IsValidShadow - 06] Shadow occupied by different Item!!"))
					return false;
				}
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
	static TArray<UMounteaInventoryItemBase*> ExcludeItems(const FItemRetrievalFilter& Filter, const TArray<UMounteaInventoryItemBase*>& ItemsToFilter);
	static TArray<UMounteaInventoryItemBase*> ExcludeItems_Impl(const FItemRetrievalFilter& Filter, const TArray<UMounteaInventoryItemBase*>& ItemsToFilter, const bool& bIsFirstCall);
#pragma endregion 
};

