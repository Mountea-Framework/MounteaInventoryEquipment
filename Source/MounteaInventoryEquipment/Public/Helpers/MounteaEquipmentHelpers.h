// Copyright Dominik Morse 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MounteaEquipmentHelpers.generated.h"

struct FMounteaEquipmentSlotCompare;
class UMounteaInstancedItem;

#define LOCTEXT_NAMESPACE "MounteaEquipmentSlots"

#pragma region EquipmentSlot

USTRUCT(BlueprintType)
struct FMounteaEquipmentSlotIdentity
{
	GENERATED_BODY()

	FMounteaEquipmentSlotIdentity() : SlotName(FText()), SlotTag(FGameplayTag()), SlotCompatibleTags(FGameplayTagContainer())
	{};

	FMounteaEquipmentSlotIdentity(const FGameplayTag& Tag) : SlotTag(Tag)
	{};
	
	FMounteaEquipmentSlotIdentity(const FText& Name, const FGameplayTag& Tag) : SlotName(Name), SlotTag(Tag)
	{};

	FMounteaEquipmentSlotIdentity(const FText& Name, const FGameplayTag& Tag, const FGameplayTagContainer& Tags) : SlotName(Name), SlotTag(Tag), SlotCompatibleTags(Tags)
	{};

	FMounteaEquipmentSlotIdentity(const FMounteaEquipmentSlotIdentity& Other) : SlotName(Other.SlotName), SlotTag(Other.SlotTag), SlotCompatibleTags(Other.SlotCompatibleTags)
	{};

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SlotTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer SlotCompatibleTags;

public:

	FString ToString() const
	{
		FString Result;

		Result.Append("Name: ");
		Result.Append(SlotName.ToString());
		Result.Append(" | ");
		Result.Append(" Tag: ");
		Result.Append(SlotTag.ToString());
		
		return Result;
	}

	FString GetSlotID() const { return SlotTag.ToString(); };

	bool IsEmpty() const
	{
		return SlotTag.IsValid() == false && SlotName.IsEmpty();
	}

	void Reset()
	{
		SlotName = FText();
		SlotTag = FGameplayTag();
		SlotCompatibleTags = FGameplayTagContainer();
	};

public:

	bool operator==(const FText& Other) const;

	bool operator==(const FGameplayTag& Other) const;

	bool operator==(const FMounteaEquipmentSlotIdentity& Other) const;
};

template<>
struct TStructOpsTypeTraits< FMounteaEquipmentSlotIdentity > : public TStructOpsTypeTraitsBase2< FMounteaEquipmentSlotIdentity >
{
	enum
	{
		WithIdenticalViaEquality = true
	};
};

FORCEINLINE  uint32 GetTypeHash(const FMounteaEquipmentSlotIdentity& Data)
{
	uint32 KeyHash = 0;

	if (Data.SlotTag.IsValid() == false) return KeyHash;
	
	KeyHash = HashCombine(KeyHash, GetTypeHash(Data.SlotTag));

	return KeyHash;
};

USTRUCT(BlueprintType, Blueprintable, DisplayName="Equipment Slot")
struct FEquipmentSlot
{
	GENERATED_BODY()

	FEquipmentSlot() : Item(nullptr), SlotGuid(FGuid()), SlotIdentity(FMounteaEquipmentSlotIdentity())
	{};

	FEquipmentSlot(const FGameplayTag& Tag) : SlotIdentity(Tag)
	{};
	
	FEquipmentSlot(const FEquipmentSlot& Other) :
	Item(Other.Item), SlotGuid(Other.SlotGuid), SlotIdentity(Other.SlotIdentity.SlotName, Other.SlotIdentity.SlotTag, Other.SlotIdentity.SlotCompatibleTags)
	{};

	FEquipmentSlot(UMounteaInstancedItem* NewItem);

public:

	/**
	 * A reference to the actual unique item that this slot contains. It points to an instance of an item, defining what item
	 * the slot is holding and providing access to its data and behaviors.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(DisplayThumbnail=false))
	TObjectPtr<UMounteaInstancedItem> Item = nullptr;

	/**
	 * The identifier for the slot. 
	 * Slot Guid represents the Item's Guid which resides in the Slot. Item Guid and Slot Guid are always the same.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid SlotGuid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(ShowOnlyInnerProperties=true))
	FMounteaEquipmentSlotIdentity SlotIdentity;

public:

	void UpdateSlot(UMounteaInstancedItem* NewItem);
	
	/** Check if the slot is empty, meaning it has no Item. */
	bool IsEmpty() const
	{ return Item == nullptr; };

	/** Checks if the slot is valid, meaning it has an item and at least one stack. */
	bool IsValid() const
	{
		const bool bIsEmpty = IsEmpty();
		const bool bHasGuid = SlotGuid.IsValid();
		const bool bHasTag = SlotIdentity.SlotTag.IsValid();
				
		return bIsEmpty && bHasGuid && bHasTag;
	}

	bool operator==(const FEquipmentSlot& Other) const;

	bool operator==(const FEquipmentSlot* Other) const;

	bool operator==(const FGuid& Other) const;
	
	bool operator==(const FText& Other) const;

	bool operator==(const UMounteaInstancedItem* Other) const;

	bool operator==(const FMounteaEquipmentSlotCompare& Other) const;
};

template<>
struct TStructOpsTypeTraits< FEquipmentSlot > : public TStructOpsTypeTraitsBase2< FEquipmentSlot >
{
	enum
	{
		WithAtomic = true,
		WithIdenticalViaEquality = true
	};
};

FORCEINLINE  uint32 GetTypeHash(const FEquipmentSlot& Data)
{
	return GetTypeHash(Data.SlotGuid);
};

USTRUCT(BlueprintType)
struct FMounteaEquipmentSlotCompare
{
	GENERATED_BODY()

	FMounteaEquipmentSlotCompare() : SlotItem(nullptr), SlotID(FText())
	{};

	FMounteaEquipmentSlotCompare(const TObjectPtr<UMounteaInstancedItem>& Item,const FGameplayTag& Tag) : SlotItem(Item), SlotTag(Tag)
	{};
	
	FMounteaEquipmentSlotCompare(const TObjectPtr<UMounteaInstancedItem>& Item, const FText& ID, const FGameplayTag& Tag) : SlotItem(Item), SlotID(ID), SlotTag(Tag)
	{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMounteaInstancedItem> SlotItem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SlotID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SlotTag;
};

#pragma endregion 

#undef LOCTEXT_NAMESPACE