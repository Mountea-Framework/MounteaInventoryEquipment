// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "Editor.h"
#include "InputCore/Classes/InputCoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "MounteaInventoryHelpers.h"

#include "MounteaInventoryEquipmentBPF.generated.h"

class UMounteaInventoryItemsTable;
class UMounteaInventoryEquipmentSettings;
class UMounteaInventoryThemeConfig;
class UMounteaInventoryConfig;
class UMounteaInventoryItemConfig;
struct FMounteaEquipmentSlotDataCompare;
struct FMounteaEquipmentSlotData;
class UMounteaEquipmentSlot;
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
	static TScriptInterface<IMounteaInventoryInterface> GetInventoryInterface(AActor* FromActor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(CompactNodeTitle="Is Valid", DisplayName="Is Valid", NativeBreakFunc))
	static bool ItemRetrievalFilter_IsValid(const FItemRetrievalFilter& Filter);

#pragma region Converter

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc, CompactNodeTitle="Array Valid"))
	static bool IsArrayValid(const TArray<UObject*>& Array);

#pragma endregion 
	
#pragma region HelpersFunctions

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "Interface"), meta=(DeterminesOutputType = "InterfaceFilter"))
	static UActorComponent* GetSingleComponentByInterface(const AActor* Target, TSubclassOf<UInterface> InterfaceFilter);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc, Keywords="valid,safe,null,check"), DisplayName="Is Valid (Inventory Ref)")
	static bool IsInventoryValid(const TScriptInterface<IMounteaInventoryInterface>& InventoryInterface);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "Object"), meta=(DeterminesOutputType = "ClassFilter"))
	static UObject* GetObjectByClass(UObject* Object, const TSubclassOf<UObject> ClassFilter, bool& bResult);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	static TArray<UMounteaInventoryThemeConfig*> GetThemeConfigs();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	static TArray<FString> GetThemeConfigsNames();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	static UMounteaInventoryThemeConfig* GetThemeConfigByName(const FString& SearchName);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory")
	static TSubclassOf<UMounteaInventoryThemeConfig> GetThemeConfigClass();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaInventoryThemeConfig"), meta=(DeterminesOutputType = "ClassFilter"))
	static UMounteaInventoryThemeConfig* GetThemeConfig(const TSubclassOf<UMounteaInventoryThemeConfig> ClassFilter, bool& bResult);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TSubclassOf<UMounteaInventoryConfig> GetItemInventoryConfigClass(const TScriptInterface<IMounteaInventoryInterface> Target, const TSubclassOf<UMounteaInventoryConfig> ClassFilter, bool& bResult);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TSubclassOf<UMounteaInventoryItemConfig> GetItemItemConfigClass(const UMounteaInventoryItemBase* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaInventoryConfig"), meta=(DeterminesOutputType = "ClassFilter"))
	static UMounteaInventoryConfig* GetInventoryConfig(const TScriptInterface<IMounteaInventoryInterface> Target, const TSubclassOf<UMounteaInventoryConfig> ClassFilter, bool& bResult);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaInventoryItemConfig"), meta=(DeterminesOutputType = "ClassFilter"))
	static UMounteaInventoryItemConfig* GetItemConfig(const UMounteaInventoryItemBase* Target, const TSubclassOf<UMounteaInventoryItemConfig> ClassFilter, bool& bResult);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaItemAdditionalData"), meta=(DeterminesOutputType = "ClassFilter"))
	static UMounteaItemAdditionalData* GetItemAdditionalData(const UMounteaInventoryItemBase* Target, const TSubclassOf<UMounteaItemAdditionalData> ClassFilter, bool& bResult);

	static UMounteaEquipmentSlot* FindEquipmentSlot(const TArray<FMounteaEquipmentSlotData>& SlotsData, const FMounteaEquipmentSlotDataCompare& Filter);

	static UMounteaEquipmentSlot* FindEquipmentSlot(const TArray<FMounteaEquipmentSlotData>& SlotsData, const FString& SlotID);
	
#pragma endregion 
	
#pragma region EditorFunctions
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool IsEditor();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool IsEditorNoPlay();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool IsShipping();

#pragma endregion 

#pragma region SettingsFunctions

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static const UMounteaInventoryEquipmentSettings* GetSettings();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static UMounteaInventoryItemsTable* GetDefaultItemsTable();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TSet<UMounteaInventoryItemCategory*> GetAllowedCategories();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TSet<UMounteaInventoryItemRarity*> GetAllowedRarities();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TArray<FKey> GetDragKeys();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TArray<FKey> GetActionRequestKeys();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TSoftClassPtr<UMounteaInventoryItemConfig> GetDefaultItemConfigClass();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool UIDebug();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool IsDragAllowed();

#pragma endregion 

#pragma region ThemeFunctions

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static FIntPoint GetInventoryDimensions();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static FIntPoint GetInventorySlotSize();

#pragma endregion 
	
#pragma region QuantityFunctions

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static int CalculateMaxSubtractQuantity(UMounteaInventoryItemBase* Item, UMounteaInventoryItemBase* OtherItem = nullptr, const int32 RequestedQuantity = 1);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static int CalculateMaxAddQuantity(UMounteaInventoryItemBase* Item, UMounteaInventoryItemBase* OtherItem = nullptr, const int32 RequestedQuantity = 1);

	static int32 AddItemQuantity(UMounteaInventoryItemBase* BaseItem, UMounteaInventoryItemBase* OtherItem = nullptr, const int32 RequestedQuantity = 1);
	static int32 RemoveItemQuantity(UMounteaInventoryItemBase* BaseItem, UMounteaInventoryItemBase* OtherItem = nullptr, const int32 RequestedQuantity = 1);

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
	static bool DoesHaveTag(const UMounteaInventoryItemBase* Item, const FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory",meta=(NativeBreakFunc, Keywords="has,have,contain"))
	static bool DoesHaveAnyTag(const UMounteaInventoryItemBase* Item, const FGameplayTagContainer Tags);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc, Keywords="has,have,contain"))
	static bool DoesHaveAllTags(const UMounteaInventoryItemBase* Item, const FGameplayTagContainer Tags);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TArray<FIntPoint> CalculateItemShadow(const FIntPoint& StartCoords, const FIntPoint& Area);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool IsValidShadow(const TArray<FIntPoint>&  SlotsCoords, const TArray<TScriptInterface<IMounteaInventorySlotWBPInterface>>& SlotsRefs, const TArray<FIntPoint>& Shadow, const TScriptInterface<IMounteaInventoryItemWBPInterface>& MovedItem);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static bool IsSafeSlot(const FIntPoint& StartCoords, const FIntPoint& Area);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Inventory", meta=(NativeBreakFunc))
	static TArray<UMounteaInventoryItemBase*> ExcludeItems(const FItemRetrievalFilter& Filter, const TArray<UMounteaInventoryItemBase*>& ItemsToFilter);
	
#pragma endregion 
};

