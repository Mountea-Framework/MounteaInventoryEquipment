// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
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
#include "Blueprint/UserWidget.h"
#include "Interfaces/Widgets/MounteaInventoryGenericWidgetInterface.h"
#include "Interfaces/Widgets/Modal/MounteaAdvancedInventoryModalContentWidgetInterface.h"
#include "MounteaAdvancedInventoryModalContentBaseWidget.generated.h"

class UMounteaModalsPayload;

/**
 * Base widget for modal content created from modal payload data.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryModalContentBaseWidget : public UUserWidget,
	public IMounteaAdvancedInventoryModalContentWidgetInterface, public IMounteaInventoryGenericWidgetInterface
{
	GENERATED_BODY()

public:

	virtual void ConstructModalContent_Implementation(UMounteaModalsPayload* Payload) override;
	virtual FOnModalContentConfirmed& GetOnModalContentConfirmedHandle() override
	{
		return OnModalContentConfirmed;
	};
	virtual FOnModalContentCancelled& GetOnModalContentCancelledHandle() override
	{
		return OnModalContentCancelled;
	};

	virtual void ProcessInventoryWidgetCommand_Implementation(const FString& Command, UObject* OptionalPayload = nullptr) override {};
	virtual void ApplyTheme_Implementation() override {};
	virtual void RefreshWidget_Implementation() override {};
	virtual void ConsumeUIInput_Implementation(const FGameplayTag& InputTag, const FMounteaWidgetInputPayload& Payload, float DeltaTime) override {};

	virtual FGameplayTag GetWidgetTag_Implementation() const override
	{
		return WidgetTag;
	};
	virtual void SetWidgetTag_Implementation(const FGameplayTag& NewWidgetTag) override
	{
		if (WidgetTag != NewWidgetTag)
			WidgetTag = NewWidgetTag;
	};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(NoResetToDefault),
		meta=(Categories="Mountea_Inventory.WidgetState.Modal,State"))
	FGameplayTag WidgetTag;

	UPROPERTY(BlueprintAssignable, Category="Events|Modal Content")
	FOnModalContentConfirmed OnModalContentConfirmed;

	UPROPERTY(BlueprintAssignable, Category="Events|Modal Content")
	FOnModalContentCancelled OnModalContentCancelled;
};
