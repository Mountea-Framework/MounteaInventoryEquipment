// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "CommonAnimatedSwitcher.h"

#include "MounteaAdvancedInventoryAnimatedSwitcher.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnimationFinished, UWidget*, ActiveWidget, int32, ActiveIndex);

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryAnimatedSwitcher : public UCommonAnimatedSwitcher
{
	GENERATED_BODY()

public:
	
	UMounteaAdvancedInventoryAnimatedSwitcher(const FObjectInitializer& ObjectInitializer);

protected:

	UFUNCTION()
	void ProcessIndexChanged(UWidget* Widget, int32 Index) const;

private:
	
#if WITH_EDITOR
	
	virtual const FText GetPaletteCategory() override;

#endif

protected:

	UPROPERTY(BlueprintAssignable, Category="AnimatedSwitcher|Event")
	FOnAnimationFinished OnAnimationFinished;
};
