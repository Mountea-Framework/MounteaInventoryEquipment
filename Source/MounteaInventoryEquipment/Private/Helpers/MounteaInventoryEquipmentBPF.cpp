// Copyright Dominik Morse 2023. All Rights Reserved.


#include "Helpers/MounteaInventoryEquipmentBPF.h"

#include "Definitions/MounteaEquipmentSlot.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Definitions/MounteaInventoryItemCategory.h"
#include "Helpers/FMounteaTemplatesLibrary.h"
#include "Helpers/MounteaEquipmentBFL.h"
#include "Interfaces/MounteaInventoryInterface.h"
#include "Interfaces/UI/MounteaInventoryItemWBPInterface.h"
#include "Interfaces/UI/MounteaInventorySlotWBPInterface.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"
#include "Settings/MounteaInventoryThemeConfig.h"
#include "Setup/MounteaInventoryConfig.h"

TScriptInterface<IMounteaInventoryInterface> UMounteaInventoryEquipmentBPF::GetInventoryInterface(AActor* FromActor)
{
	return GetInterfaceFrom<UMounteaInventoryInterface, TScriptInterface<IMounteaInventoryInterface>>(FromActor);
}

bool UMounteaInventoryEquipmentBPF::ItemRetrievalFilter_IsValid(const FItemRetrievalFilter& Filter)
{
	return Filter.IsValid();
}

bool UMounteaInventoryEquipmentBPF::IsArrayValid(const TArray<UObject*>& Array)
{
	return Array.Num() > 0;
}

UActorComponent* UMounteaInventoryEquipmentBPF::GetSingleComponentByInterface(const AActor* Target, TSubclassOf<UInterface> InterfaceFilter)
{
	if (Target == nullptr) return nullptr;

	TArray<UActorComponent*> TempComps = Target->GetComponentsByInterface(InterfaceFilter);

	if (TempComps.IsEmpty()) return nullptr;

	return TempComps[0];
}

bool UMounteaInventoryEquipmentBPF::IsInventoryValid(	const TScriptInterface<IMounteaInventoryInterface>& InventoryInterface)
{
	return InventoryInterface.GetObject() != nullptr;
}

UObject* UMounteaInventoryEquipmentBPF::GetObjectByClass(UObject* Object, const TSubclassOf<UObject> ClassFilter,	bool& bResult)
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

TArray<UMounteaInventoryThemeConfig*> UMounteaInventoryEquipmentBPF::GetThemeConfigs()
{
	TArray<UMounteaInventoryThemeConfig*> Result;
	if (const auto Settings = GetSettings())
	{
		for (auto& Itr : Settings->ThemeConfigs)
		{
			if (Itr.IsNull()) continue;
			
			Result.Add(Itr.LoadSynchronous());
		}
	}

	return Result;
}

TArray<FString> UMounteaInventoryEquipmentBPF::GetThemeConfigsNames()
{
	TArray<FString> Result;

	for (const auto& Itr : GetThemeConfigs())
	{
		
		if (Itr) Result.Add(Itr->GetName());
	}
	return Result;
}

UMounteaInventoryThemeConfig* UMounteaInventoryEquipmentBPF::GetThemeConfigByName(const FString& SearchName)
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

TSubclassOf<UMounteaInventoryThemeConfig> UMounteaInventoryEquipmentBPF::GetThemeConfigClass()
{
	if (GetSettings()->ThemeConfig) return GetSettings()->ThemeConfig->StaticClass();

	return nullptr;
}

UMounteaInventoryThemeConfig* UMounteaInventoryEquipmentBPF::GetThemeConfig(const TSubclassOf<UMounteaInventoryThemeConfig> ClassFilter, bool& bResult)
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

UContentTheme* UMounteaInventoryEquipmentBPF::GetContentThemeConfig(const TSubclassOf<UContentTheme> ClassFilter, bool& bResult)
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

TSubclassOf<UMounteaInventoryConfig> UMounteaInventoryEquipmentBPF::GetItemInventoryConfigClass(const TScriptInterface<IMounteaInventoryInterface> Target, const TSubclassOf<UMounteaInventoryConfig> ClassFilter, bool& bResult)
{
	if (!Target) return nullptr;

	return Target->Execute_GetInventoryConfigClass(Target.GetObject());
}

TSubclassOf<UMounteaInventoryItemConfig> UMounteaInventoryEquipmentBPF::GetItemItemConfigClass(const UMounteaInventoryItemBase* Target)
{
	if (!Target) return nullptr;

	return Target->GetItemConfigClass();
}

UMounteaInventoryConfig* UMounteaInventoryEquipmentBPF::GetInventoryConfig(const TScriptInterface<IMounteaInventoryInterface> Target, const TSubclassOf<UMounteaInventoryConfig> ClassFilter, bool& bResult)
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

UMounteaInventoryItemConfig* UMounteaInventoryEquipmentBPF::GetItemConfig(const UMounteaInventoryItemBase* Target, const TSubclassOf<UMounteaInventoryItemConfig> ClassFilter, bool& bResult)
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

bool UMounteaInventoryEquipmentBPF::IsEditor()
{
#if WITH_EDITOR
	return true;
#endif

	return false;
}

bool UMounteaInventoryEquipmentBPF::IsEditorNoPlay()
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

bool UMounteaInventoryEquipmentBPF::IsShipping()
{
		
#if UE_BUILD_SHIPPING
		return true;
#endif

	return false;
}

const UMounteaInventoryEquipmentSettings* UMounteaInventoryEquipmentBPF::GetSettings()
{
	return GetDefault<UMounteaInventoryEquipmentSettings>();
}

UMounteaInventoryItemsTable* UMounteaInventoryEquipmentBPF::GetDefaultItemsTable()
{
	return GetSettings()->DefaultInventoryItemDefinitionsTable.LoadSynchronous();
}

TSet<UMounteaInventoryItemCategory*> UMounteaInventoryEquipmentBPF::GetAllowedCategories()
{
	TSet<UMounteaInventoryItemCategory*> ReturnValues;
	for (auto Itr : GetSettings()->InventoryCategories)
	{
		ReturnValues.Add(Itr.LoadSynchronous());
	}

	return ReturnValues;
}

TSet<UMounteaInventoryItemRarity*> UMounteaInventoryEquipmentBPF::GetAllowedRarities()
{
	TSet<UMounteaInventoryItemRarity*> ReturnValues;
	for (auto Itr : GetSettings()->InventoryRarities)
	{
		ReturnValues.Add(Itr.LoadSynchronous());
	}

	return ReturnValues;
}

TArray<FKey> UMounteaInventoryEquipmentBPF::GetDragKeys()
{
	return GetSettings()->DragKeys;
}

TArray<FKey> UMounteaInventoryEquipmentBPF::GetActionRequestKeys()
{
	return GetSettings()->ActionRequestKeys;
}

TSoftClassPtr<UMounteaInventoryItemConfig> UMounteaInventoryEquipmentBPF::GetDefaultItemConfigClass()
{
	return GetSettings()->DefaultItemConfigClass;
}

bool UMounteaInventoryEquipmentBPF::UIDebug()
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

bool UMounteaInventoryEquipmentBPF::IsDragAllowed()
{
	return GetSettings()->bDragDropAllowed;
}

FIntPoint UMounteaInventoryEquipmentBPF::GetInventoryDimensions()
{
	bool bFound;
	const auto Config = GetThemeConfig(UMounteaInventoryThemeConfig::StaticClass(), bFound);
	return bFound ? Config->InventoryBaseSize : FIntPoint(6, 10);
}

FIntPoint UMounteaInventoryEquipmentBPF::GetInventorySlotSize()
{
	bool bFound;
	const auto Config = GetThemeConfig(UMounteaInventoryThemeConfig::StaticClass(), bFound);
	return bFound ? Config->SlotBaseSize : FIntPoint(16, 16);
}



TArray<FIntPoint> UMounteaInventoryEquipmentBPF::CalculateItemShadow(const FIntPoint& StartCoords,
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

bool UMounteaInventoryEquipmentBPF::IsValidShadow(const TArray<FIntPoint>& SlotsCoords,
	const TArray<TScriptInterface<IMounteaInventorySlotWBPInterface>>& SlotsRefs, const TArray<FIntPoint>& Shadow,
	const TScriptInterface<IMounteaInventoryItemWBPInterface>& MovedItem)
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

bool UMounteaInventoryEquipmentBPF::IsSafeSlot(const FIntPoint& StartCoords, const FIntPoint& Area)
{
	CalculateItemShadow(StartCoords, Area);

	return false;
}

TArray<UMounteaInventoryItemBase*> UMounteaInventoryEquipmentBPF::ExcludeItems(const FItemRetrievalFilter& Filter, const TArray<UMounteaInventoryItemBase*>& ItemsToFilter)
{
	if (!Filter.IsValid()) return TArray<UMounteaInventoryItemBase*>();

	TArray<UMounteaInventoryItemBase*> TempResult;

	for (const auto& Itr : ItemsToFilter)
	{
		if (!Itr) continue;

		bool bExclude = false;

		/* BREAKING
		if (Filter.bSearchByTag && Itr->GetTags().HasAny(Filter.Tags))
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
		*/

		if (!bExclude)
		{
			TempResult.Add(Itr);
		}
	}

	return TempResult;
}
