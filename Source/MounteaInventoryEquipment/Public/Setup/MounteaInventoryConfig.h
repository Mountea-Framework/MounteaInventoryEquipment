// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "MounteaInventoryConfig.generated.h"

/*
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
*/

/**
 * 
 */
UCLASS( Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Inventory Config (Base)")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryConfig : public UObject
{
	GENERATED_BODY()

	UMounteaInventoryConfig();
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="1. Required")
	FGameplayTagContainer DefaultInventoryFlags;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category="2. Debug", meta=(NoResetToDefault))
	FGameplayTagContainer InventoryFlags;

	UPROPERTY(VisibleAnywhere, Category="2. Debug")
	int32 RepKey = 0;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|InventoryConfig")
	FORCEINLINE FGameplayTagContainer GetDefaultInventoryFlags() const
	{ return DefaultInventoryFlags; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|InventoryConfig")
	FORCEINLINE FGameplayTagContainer GetInventoryFlags() const
	{ return InventoryFlags; };
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|InventoryConfig")
	FORCEINLINE bool HasFlag(const FGameplayTag Flag) const
	{
		return InventoryFlags.HasTag(Flag);
	};

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|InventoryConfig")
	FORCEINLINE bool DoesNotHasFlag(const FGameplayTag Flag) const
	{
		return !InventoryFlags.HasTag(Flag);
	};

	UFUNCTION(BlueprintCallable, Category="Mountea|InventoryConfig")
	void RemoveFlags(const FGameplayTagContainer& Flags);

	UFUNCTION(BlueprintCallable, Category="Mountea|InventoryConfig")
	void AddFlags(const FGameplayTagContainer& Flags);
	
	UFUNCTION(BlueprintCallable, Category="Mountea|InventoryConfig")
	void RemoveFlag(const FGameplayTag& Flag, const bool bSkipNetUpdate = false);

	UFUNCTION(BlueprintCallable, Category="Mountea|InventoryConfig")
	void AddFlag(const FGameplayTag& Flag, const bool bSkipNetUpdate = false);

	UFUNCTION(BlueprintCallable, Category="Mountea|InventoryConfig")
	void ResetFlags();
	
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