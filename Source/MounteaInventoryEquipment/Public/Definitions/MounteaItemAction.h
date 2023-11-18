// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Helpers/MounteaInventoryHelpers.h"

#include "MounteaItemAction.generated.h"


struct FMounteaDynamicDelegateContext;
struct FGameplayTag;
class UMounteaInstancedItem;

/**
 * 
 */
UCLASS( Abstract, Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Item Action (Base)")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemAction : public UObject
{
	GENERATED_BODY()

	UMounteaInventoryItemAction() : bRequiresAuthority(1), World(nullptr) {};

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|ItemAction")
	void InitializeAction(UMounteaInstancedItem* ItemInFocus, FMounteaDynamicDelegateContext Context);
	void InitializeAction_Implementation(UMounteaInstancedItem* ItemInFocus, FMounteaDynamicDelegateContext Context);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|ItemAction", DisplayName="Can Display Action")
	bool DisplayAction(UMounteaInstancedItem* ItemInFocus) const;
	bool DisplayAction_Implementation(UMounteaInstancedItem* ItemInFocus) const;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|ItemAction")
	FInventoryUpdateResult ProcessAction(UMounteaInstancedItem* ItemInFocus);
	
public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|ItemAction")
	FORCEINLINE FGameplayTag GetActionTag() const
	{ return ActionTag; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|ItemAction")
	FORCEINLINE FName GetActionName() const
	{ return ActionName; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|ItemAction")
	FORCEINLINE UTexture2D* GetActionIcon() const
	{ return ActionIcon; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|ItemAction")
	FORCEINLINE UMounteaInstancedItem* GetOwningItem() const
	{
		return OwningItem;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|ItemAction")
	FORCEINLINE FMounteaDynamicDelegateContext GetActionContext() const
	{ return ActionContext; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|ItemAction")
	FORCEINLINE bool DoesRequireAuthority() const
	{ return bRequiresAuthority; };

public:
	
	void CopyFromOther(const UMounteaInventoryItemAction* OtherAction);
	
public:
	
	UFUNCTION(BlueprintCallable, Category="Mountea|ItemAction")
	virtual void SetWorldFromLevel(ULevel* FromLevel);

	virtual void SetWorld(UWorld* NewWorld);

	FORCEINLINE ULevel* GetLevel() const
	{
		return GetTypedOuter<ULevel>();
	}
	
	virtual UWorld* GetWorld() const override
	{
		if (World) return World;
		
		// CDO objects do not belong to a world
		// If the actors outer is destroyed or unreachable we are shutting down and the world should be nullptr
		if (
			!HasAnyFlags(RF_ClassDefaultObject) && ensureMsgf(GetOuter(), TEXT("Actor: %s has a null OuterPrivate in AActor::GetWorld()"), *GetFullName())
			&& !GetOuter()->HasAnyFlags(RF_BeginDestroyed) && !GetOuter()->IsUnreachable()
			)
		{
			if (ULevel* Level = GetLevel())
			{
				return Level->OwningWorld;
			}
		}
		return nullptr;
	}

protected:

	virtual bool IsSupportedForNetworking() const override {	return true; };
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="1. Required", meta=(NoResetToDefault))
	FGameplayTag ActionTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="1. Required", meta=(NoResetToDefault))
	FName ActionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="2. Optional", meta=(NoResetToDefault, ExposeOnSpawn))
	UTexture2D* ActionIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="1. Required", meta=(NoResetToDefault))
	uint8 bRequiresAuthority : 1;
	
private:

	UPROPERTY(Transient, VisibleAnywhere, Category="3. Debug", meta=(DisplayThumbnail=false), AdvancedDisplay)
	FMounteaDynamicDelegateContext ActionContext;;

	UPROPERTY(VisibleAnywhere, Category="3. Debug", meta=(DisplayThumbnail=false), AdvancedDisplay)
	class UWorld* World;

	UPROPERTY(VisibleAnywhere, Category="3. Debug", meta=(DisplayThumbnail=false), AdvancedDisplay)
	UMounteaInstancedItem* OwningItem = nullptr;
};

USTRUCT(BlueprintType)
struct FMounteaItemAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Inventory", meta=(NoResetToDefault, AllowAbstract=false, BlueprintBaseOnly=true))
	UMounteaInventoryItemAction* ItemAction = nullptr;

public:
	
	bool operator==(const FMounteaItemAction& Other) const
	{
		if (!ItemAction)
		{
			return true;
		}

		if (!Other.ItemAction)
		{
			return true;
		}

		return
		ItemAction->GetActionTag() == Other.ItemAction->GetActionTag() &&
		ItemAction->GetActionName() == Other.ItemAction->GetActionName();
	}

	bool operator!=(const FMounteaItemAction& Other) const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FMounteaItemAction& Src)
	{
		if (Src.ItemAction)
		{
			const uint32 Hash = GetTypeHash(Src.ItemAction->GetActionTag()) + GetTypeHash(Src.ItemAction->GetActionName());
			return Hash;
		}
		return 0;
	}
};

