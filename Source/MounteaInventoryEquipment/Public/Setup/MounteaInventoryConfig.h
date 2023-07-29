// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MounteaInventoryConfig.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EMounteaInventoryConfig: uint8
{
	Default				= 0 UMETA(Hidden),
	EMIC_Store		= 1 << 0 UMETA(DisplayName="Store"),
	EMIC_Loot		= 1 << 1 UMETA(DisplayName="Loot"),
	EMIC_Buy		= 1 << 2 UMETA(DisplayName="Buy"),
	EMIC_Sell			= 1 << 3 UMETA(DisplayName="Sell"),
	EMIC_Steal		= 1 << 4 UMETA(DisplayName="Steal"),
};

ENUM_CLASS_FLAGS(EMounteaInventoryConfig);

/**
 * 
 */
UCLASS( Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Inventory Config (Base)")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryConfig : public UObject
{
	GENERATED_BODY()

	UMounteaInventoryConfig();
	
public:

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = EMounteaInventoryConfig), Category="1. Required")
	uint8  InventoryFlags = 0;

	UPROPERTY(VisibleAnywhere, Category="2. Debug")
	int32 RepKey = 0;

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|InventoryConfig")
	FORCEINLINE bool HasFlag(UPARAM(meta = (Bitmask, BitmaskEnum = EMounteaInventoryConfig)) const uint8 Flag) const
	{
		return (Flag & InventoryFlags) == Flag;
	};

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|InventoryConfig")
	FORCEINLINE bool IsMatch(UPARAM(meta = (Bitmask, BitmaskEnum = EMounteaInventoryConfig)) const uint8 Flag, const uint8 Flags) const
	{
		return (Flag & Flags) == Flag;
	};

	UFUNCTION(BlueprintCallable, Category="Mountea|InventoryConfig")
	void RemoveFlag(UPARAM(meta = (Bitmask, BitmaskEnum = EMounteaInventoryConfig)) const uint8 Flag);

	UFUNCTION(BlueprintCallable, Category="Mountea|InventoryConfig")
	void AddFlag(UPARAM(meta = (Bitmask, BitmaskEnum = EMounteaInventoryConfig)) const uint8 Flag);

public:

	inline int32 GetRepKey() const { return RepKey; };
	
protected:

	virtual bool IsSupportedForNetworking() const override {	return true; };
	
};

USTRUCT(BlueprintType)
struct FMounteaInventoryConfigBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Inventory", meta=(NoResetToDefault, AllowAbstract=false, BlueprintBaseOnly=true))
	UMounteaInventoryConfig* MounteaInventoryConfig = nullptr;
};