// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Helpers/BlueprintFunctionLibraries/MounteaInventoryEquipmentBFL.h"

#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaInventoryItemCategory.h"
#include "Helpers/FMounteaTemplatesLibrary.h"
#include "Helpers/BlueprintFunctionLibraries/MounteaEquipmentBFL.h"
#include "Interfaces/MounteaInventoryInterface.h"
#include "Interfaces/UI/MounteaInventoryItemWBPInterface.h"
#include "Interfaces/UI/MounteaInventorySlotWBPInterface.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"
#include "Settings/MounteaInventoryThemeConfig.h"
#include "Settings/Config/MounteaDefaultsConfig.h"
#include "Setup/MounteaInventoryConfig.h"

TScriptInterface<IMounteaInventoryInterface> UMounteaInventoryEquipmentBFL::GetInventoryInterface(AActor* FromActor)
{
	return GetInterfaceFrom<UMounteaInventoryInterface, TScriptInterface<IMounteaInventoryInterface>>(FromActor);
}

bool UMounteaInventoryEquipmentBFL::ItemRetrievalFilter_IsValid(const FItemRetrievalFilter& Filter)
{
	return Filter.IsValid();
}

bool UMounteaInventoryEquipmentBFL::IsArrayValid(const TArray<UObject*>& Array)
{
	return Array.Num() > 0;
}

UActorComponent* UMounteaInventoryEquipmentBFL::GetSingleComponentByInterface(const AActor* Target, TSubclassOf<UInterface> InterfaceFilter)
{
	if (Target == nullptr) return nullptr;

	TArray<UActorComponent*> TempComps = Target->GetComponentsByInterface(InterfaceFilter);

	if (TempComps.IsEmpty()) return nullptr;

	return TempComps[0];
}

bool UMounteaInventoryEquipmentBFL::IsInventoryValid(	const TScriptInterface<IMounteaInventoryInterface>& InventoryInterface)
{
	return InventoryInterface.GetObject() != nullptr;
}

UObject* UMounteaInventoryEquipmentBFL::GetObjectByClass(UObject* Object, const TSubclassOf<UObject> ClassFilter,	bool& bResult)
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

TArray<UMounteaInventoryThemeConfig*> UMounteaInventoryEquipmentBFL::GetThemeConfigs()
{
	TArray<UMounteaInventoryThemeConfig*> Result;
	if (const auto Settings = GetSettings())
	{
		for (auto& Itr : Settings->GetThemeConfigs())
		{
			if (Itr.IsNull()) continue;
			
			Result.Add(Itr.LoadSynchronous());
		}
	}

	return Result;
}

TArray<FString> UMounteaInventoryEquipmentBFL::GetThemeConfigsNames()
{
	TArray<FString> Result;

	for (const auto& Itr : GetThemeConfigs())
	{
		
		if (Itr) Result.Add(Itr->GetName());
	}
	return Result;
}

UMounteaInventoryThemeConfig* UMounteaInventoryEquipmentBFL::GetThemeConfigByName(const FString& SearchName)
{
	UMounteaInventoryThemeConfig* Result = nullptr;

	for (const auto& Itr : GetThemeConfigs())
	{
		if (Itr && Itr->GetName().Equals(SearchName))
		{
			Result = Itr;

			return Result;
		}
	}
	
	return Result;
}

TSubclassOf<UMounteaInventoryThemeConfig> UMounteaInventoryEquipmentBFL::GetThemeConfigClass()
{
	if (GetDefaults() && GetDefaults()->DefaultInventoryThemeConfigClass.Get()) return GetDefaults()->DefaultInventoryThemeConfigClass.LoadSynchronous()->StaticClass();

	return nullptr;
}

UMounteaInventoryThemeConfig* UMounteaInventoryEquipmentBFL::GetThemeConfig(const TSubclassOf<UMounteaInventoryThemeConfig> ClassFilter, bool& bResult)
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

	if (GetDefaults()->DefaultInventoryThemeConfigClass.IsNull())
	{
		return NewObject<UMounteaInventoryThemeConfig>(GetTransientPackage(), ClassFilter);
	}

	UMounteaInventoryThemeConfig* const NewTheme = NewObject<UMounteaInventoryThemeConfig>(GetTransientPackage(), GetDefaults()->DefaultInventoryThemeConfigClass.LoadSynchronous());
	
	return NewTheme;
}

UContentTheme* UMounteaInventoryEquipmentBFL::GetContentThemeConfig(const TSubclassOf<UContentTheme> ClassFilter, bool& bResult)
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
		return NewObject<UContentTheme>(GetTransientPackage(), ClassFilter);
	}
		
	bResult = false;
	return nullptr;
}

TSubclassOf<UMounteaInventoryConfig> UMounteaInventoryEquipmentBFL::GetItemInventoryConfigClass(const TScriptInterface<IMounteaInventoryInterface> Target, const TSubclassOf<UMounteaInventoryConfig> ClassFilter, bool& bResult)
{
	if (!Target) return nullptr;

	return Target->Execute_GetInventoryConfigClass(Target.GetObject());
}

TSubclassOf<UMounteaInventoryItemConfig> UMounteaInventoryEquipmentBFL::GetItemItemConfigClass(const UMounteaInventoryItemBase* Target)
{
	if (!Target) return nullptr;

	return Target->GetItemConfigClass();
}

UMounteaInventoryConfig* UMounteaInventoryEquipmentBFL::GetInventoryConfig(const TScriptInterface<IMounteaInventoryInterface> Target, const TSubclassOf<UMounteaInventoryConfig> ClassFilter, bool& bResult)
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

UMounteaInventoryItemConfig* UMounteaInventoryEquipmentBFL::GetItemConfig(const UMounteaInventoryItemBase* Target, const TSubclassOf<UMounteaInventoryItemConfig> ClassFilter, bool& bResult)
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

bool UMounteaInventoryEquipmentBFL::IsEditor()
{
#if WITH_EDITOR
	return true;
#endif

	return false;
}

bool UMounteaInventoryEquipmentBFL::IsEditorNoPlay()
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

bool UMounteaInventoryEquipmentBFL::IsShipping()
{
		
#if UE_BUILD_SHIPPING
		return true;
#endif

	return false;
}

UMounteaInventoryEquipmentSettings* UMounteaInventoryEquipmentBFL::GetSettings()
{
	return GetMutableDefault<UMounteaInventoryEquipmentSettings>();
}

UMounteaDefaultsConfig* UMounteaInventoryEquipmentBFL::GetDefaults()
{
	return GetSettings()->GetDefaultsConfig().LoadSynchronous();
}

TSet<UMounteaInventoryItemCategory*> UMounteaInventoryEquipmentBFL::GetAllowedCategories()
{
	TSet<UMounteaInventoryItemCategory*> ReturnValues;
	for (auto Itr : GetSettings()->GetInventoryCategories())
	{
		ReturnValues.Add(Itr.LoadSynchronous());
	}

	return ReturnValues;
}

TSet<UMounteaInventoryItemRarity*> UMounteaInventoryEquipmentBFL::GetAllowedRarities()
{
	TSet<UMounteaInventoryItemRarity*> ReturnValues;
	for (auto Itr : GetSettings()->GetInventoryRarities())
	{
		ReturnValues.Add(Itr.LoadSynchronous());
	}

	return ReturnValues;
}

TArray<FKey> UMounteaInventoryEquipmentBFL::GetDragKeys()
{
	return GetSettings()->DragKeys;
}

TArray<FKey> UMounteaInventoryEquipmentBFL::GetActionRequestKeys()
{
	return GetSettings()->ActionRequestKeys;
}

bool UMounteaInventoryEquipmentBFL::UIDebug()
{
	if (IsShipping())
	{
		return false;
	}
		
	if (const UMounteaInventoryEquipmentSettings* Settings = GetDefault<UMounteaInventoryEquipmentSettings>())
	{
		return Settings->IsDebugEnabled();
	}

	return false;
}

TArray<FIntPoint> UMounteaInventoryEquipmentBFL::CalculateItemShadow(const FIntPoint& StartCoords,
	const FIntPoint& Area)
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

bool UMounteaInventoryEquipmentBFL::IsValidShadow(const TArray<FIntPoint>& SlotsCoords,
	const TArray<TScriptInterface<IMounteaInventorySlotWBPInterface>>& SlotsRefs, const TArray<FIntPoint>& Shadow,
	const TScriptInterface<IMounteaInventoryItemWBPInterface>& MovedItem)
{
	if (MovedItem.GetObject() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[IsValidShadow - 00] Trying to move nothing!"))
		return false;
	}

	const FIntPoint Area = MovedItem->Execute_GetItemDimensions(MovedItem.GetObject());

		
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

bool UMounteaInventoryEquipmentBFL::IsSafeSlot(const FIntPoint& StartCoords, const FIntPoint& Area)
{
	CalculateItemShadow(StartCoords, Area);

	return false;
}

TArray<UMounteaInstancedItem*> UMounteaInventoryEquipmentBFL::ExcludeItems(const FItemRetrievalFilter& Filter, const TArray<UMounteaInstancedItem*>& ItemsToFilter)
{
	if (!Filter.IsValid()) return ItemsToFilter;

	TArray<UMounteaInstancedItem*> TempResult;

	for (const auto& Itr : ItemsToFilter)
	{
		if (!Itr) continue;

		bool bExclude = false;
		
		if (Filter.bSearchByTag && Itr->GetItemFlags().HasAny(Filter.Tags))
		{
			bExclude = true;
		}
		else if (Filter.bSearchByItem && Itr == Filter.Item)
		{
			bExclude = true;
		}
		else if (Filter.bSearchByClass && Itr->IsA(Filter.Class))
		{
			bExclude = true;
		}
		else if (Filter.bSearchByGUID && Itr->GetGuid() == Filter.Guid)
		{
			bExclude = true;
		}

		if (!bExclude)
		{
			TempResult.Add(Itr);
		}
	}

	return TempResult;
}
