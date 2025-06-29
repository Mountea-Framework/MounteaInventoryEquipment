// Copyright (C) 2025 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not 
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Primary Data", DuplicateTransient)
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
