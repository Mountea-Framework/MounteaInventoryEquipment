// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "MounteaItemAction.generated.h"

struct FGameplayTag;
class UMounteaInventoryItemBase;
/**
 * 
 */
UCLASS( Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Item Action (Base)")
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryItemAction : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|ItemAction")
	void InitializeAction(UMounteaInventoryItemBase* ItemInFocus);
	void InitializeAction_Implementation(UMounteaInventoryItemBase* ItemInFocus);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|ItemAction")
	bool DisplayAction(UMounteaInventoryItemBase* ItemInFocus) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Mountea|ItemAction")
	void ProcessAction(UMounteaInventoryItemBase* ItemInFocus);

public:

	UFUNCTION(BlueprintCallable, Category="Mountea|ItemAction")
	FORCEINLINE FGameplayTag GetActionTag() const
	{ return ActionTag; };

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="1. Required")
	FGameplayTag ActionTag;
	
private:

	UPROPERTY(VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false))
	class UWorld* World;

	UPROPERTY(VisibleAnywhere, Category="2. Debug", meta=(DisplayThumbnail=false))
	UMounteaInventoryItemBase* OwningItem = nullptr;
};

USTRUCT(BlueprintType)
struct FMounteaItemAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Inventory", meta=(NoResetToDefault, AllowAbstract=false, BlueprintBaseOnly=true))
	UMounteaInventoryItemAction* ItemAction = nullptr;
};