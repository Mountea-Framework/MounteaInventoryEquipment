// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "MounteaInventoryItemEffect.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Mountea), Abstract, BlueprintType, Blueprintable, DisplayName="Inventory Item Effect")
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryItemEffect : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data")
	FGuid EffectGuid;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	FText EffectName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	FGameplayTagContainer EffectTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data")
	float EffectDuration;

private:

	// TODO: Should effect know its applying Action?
	UPROPERTY(BlueprintReadOnly, Category="Secondary Data", meta=(AllowPrivateAccess))
	TObjectPtr<UObject> Initiator;

public:

	UFUNCTION(BlueprintNativeEvent, Category="Inventory|Item|Effect")
	void ApplyEffect();
	virtual void ApplyEffect_Implementation() {};
};
