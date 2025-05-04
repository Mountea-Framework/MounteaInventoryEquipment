// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "MounteaInventoryItemAction.generated.h"

class UMounteaInventoryItemEffect;
struct FInventoryItem;

/**
 * 
 */
UCLASS(ClassGroup=(Mountea), Abstract, BlueprintType, Blueprintable, DisplayName="Inventory Item Action")
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryItemAction : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data", DuplicateTransient)
	FGuid ActionGuid;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	FText ActionName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	FGameplayTagContainer ActionTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	TSet<TSoftClassPtr<UMounteaInventoryItemEffect>> ActionEffects;
	
public:
	
	// TODO: BlueprintCallable from Statics to K2 Node can be applied
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory|Item|Action", meta=(CustomTag="MounteaK2Validate"))
	bool CanExecuteAction(UObject* ActionInitiator, const FInventoryItem& TargetItem) const;
	virtual bool CanExecuteAction_Implementation(UObject* ActionInitiator, const FInventoryItem& TargetItem) const;

	// TODO: BlueprintCallable from Statics to K2 Node can be applied
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory|Item|Action", meta=(CustomTag="MounteaK2Setter"))
	void ExecuteAction(UObject* ActionInitiator, const FInventoryItem& TargetItem);
	virtual void ExecuteAction_Implementation(UObject* ActionInitiator, const FInventoryItem& TargetItem);

	// TODO: BlueprintCallable from Statics to K2 Node can be applied
	UFUNCTION(BlueprintNativeEvent, Category="Mountea|Inventory|Item|Action", meta=(CustomTag="MounteaK2Setter"))
	void FinishAction();
	virtual void FinishAction_Implementation();

private:

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="Secondary Data", meta=(AllowPrivateAccess))
	TObjectPtr<UObject> Initiator;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category="Secondary Data", meta=(AllowPrivateAccess))
	FGuid ItemGuid;
};
