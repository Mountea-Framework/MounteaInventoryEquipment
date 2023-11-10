// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "MounteaItemHelpers.generated.h"

class UMounteaInstancedItem;

UENUM(BlueprintType)
enum class EItemDataSource : uint8
{
	EIDS_SourceTable				UMETA(DisplayName="Source Table"),
	EIDS_SourceItem				UMETA(DisplayName="Source Item"),

	Default									UMETA(hidden)
};

USTRUCT(BlueprintType, Blueprintable, DisplayName="Item Slot Stack")
struct FItemSlotStack
{
	GENERATED_BODY()

	// Default constructor with no parameters.
	FItemSlotStack()
		: StackSize(0), StackGuid(FGuid::NewGuid()), SlotGuid(FGuid())
	{};

	// Constructor with parameters to initialize the stack.
	FItemSlotStack(const int32 InStackSize, const FGuid&InStackGuid, const FGuid&InSlotGuid)
		: StackSize(InStackSize), StackGuid(InStackGuid), SlotGuid(InSlotGuid)
	{};

	// Convenience constructor for creating a stack with a size and parent Slot but generating new GUID.
	explicit FItemSlotStack(const int32 InStackSize, const FGuid& InSlotGuid)
		: StackSize(InStackSize), SlotGuid(InSlotGuid)
	{};

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 StackSize = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGuid StackGuid = FGuid::NewGuid();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGuid SlotGuid;

public:

	// Checks if the stack size is valid (greater than zero).
	bool IsValidStackSize() const
	{
		return StackSize > 0;
	};

	// Checks if the stack has valid GUIDs (not zero GUIDs).
	bool HasValidGuids() const
	{
		return StackGuid.IsValid() && SlotGuid.IsValid();
	};

	// Full validation check for the stack.
	bool IsValid() const
	{
		return IsValidStackSize() && HasValidGuids();
	};

	friend FArchive& operator<<(FArchive& Ar, FItemSlotStack& Element);
	
	bool Serialize(class FArchive& Ar);
	
	bool operator==(const FItemSlotStack& Other) const;
};


USTRUCT(BlueprintType, Blueprintable, DisplayName="Item Slot")
struct FItemSlot
{
	GENERATED_BODY()

	FItemSlot() : Item(nullptr)
	{};
	
	FItemSlot(UMounteaInstancedItem* NewItem, const TSet<FItemSlotStack>& NewStacks)
		: Item(NewItem), Stacks(NewStacks)
	{};

	explicit FItemSlot(UMounteaInstancedItem* NewItem)
		: Item(NewItem)
	{};

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(DisplayThumbnail=false))
	UMounteaInstancedItem* Item = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSet<FItemSlotStack> Stacks;

public:

	/** Checks if the slot is valid, meaning it has an item and at least one stack. */
	bool IsValid() const
	{
		return Item != nullptr && Stacks.Num() > 0;
	}

	/** Validates the consistency of stack GUIDs, ensuring they match with the item's GUID. */
	bool AreStacksValid() const;

	/** Validates the total stack size, ensuring it does not exceed the item's maximum stack size. */
	bool IsStackSizeValid() const;


	bool operator==(const FItemSlot& Other) const;

	bool operator==(const FGuid& Other) const;

	bool operator==(const UMounteaInstancedItem* Other) const;

	friend FArchive& operator<<(FArchive& Ar, FItemSlot& Element);
	
	bool Serialize(class FArchive& Ar);
	
};

template<>
struct TStructOpsTypeTraits< FItemSlotStack > : public TStructOpsTypeTraitsBase2< FItemSlotStack >
{
	enum
	{
		WithSerializer = true,
		WithAtomic = true,
		WithIdenticalViaEquality = true
	};
};

template<>
struct TStructOpsTypeTraits< FItemSlot > : public TStructOpsTypeTraitsBase2< FItemSlot >
{
	enum
	{
		WithSerializer = true,
		WithAtomic = true,
		WithIdenticalViaEquality = true
	};
};

FORCEINLINE  uint32 GetTypeHash(const FItemSlotStack& Data)
{
	return GetTypeHash(Data.StackGuid);
};

FORCEINLINE  uint32 GetTypeHash(const FItemSlot& Data)
{
	return PointerHash(Data.Item);
};