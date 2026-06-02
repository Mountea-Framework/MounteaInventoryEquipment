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
#include "Interfaces/Widgets/Modal/MounteaAdvancedInventoryModalWidgetInterface.h"
#include "MounteaAdvancedInventoryModalBaseWidget.generated.h"

class UPanelWidget;
class UMounteaModalsPayload;

/**
 * Base modal window widget capable of hosting modal content widgets.
 */
UCLASS(Blueprintable, BlueprintType)
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryModalBaseWidget : public UUserWidget,
	public IMounteaAdvancedInventoryModalWidgetInterface, public IMounteaInventoryGenericWidgetInterface
{
	GENERATED_BODY()

public:

	virtual void AddModalContentToModalWindow_Implementation(UUserWidget* ModalContentWidget, UMounteaModalsPayload* Payload) override;
	virtual void ModalContentConfirmed_Implementation(UObject* Payload) override {};
	virtual void ModalContentCancelled_Implementation() override {};
	virtual FOnModalContentAddedToModalWindow& GetOnModalContentAddedToModalWindowHandle() override
	{
		return OnModalContentAddedToModalWindow;
	};
	virtual FOnModalConfirmed& GetOnModalConfirmedHandle() override
	{
		return OnModalConfirmed;
	};
	virtual FOnModalCancelled& GetOnModalCancelledHandle() override
	{
		return OnModalCancelled;
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

	UPROPERTY(BlueprintReadOnly, Category="Widgets|Modal")
	TObjectPtr<UUserWidget> CurrentModalContentWidget = nullptr;

	UPROPERTY(BlueprintAssignable, Category="Events|Modal")
	FOnModalContentAddedToModalWindow OnModalContentAddedToModalWindow;

	UPROPERTY(BlueprintAssignable, Category="Events|Modal")
	FOnModalConfirmed OnModalConfirmed;

	UPROPERTY(BlueprintAssignable, Category="Events|Modal")
	FOnModalCancelled OnModalCancelled;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Primary Data",
		meta=(NoResetToDefault),
		meta=(Categories="Mountea_Inventory.WidgetState.Modal,State"))
	FGameplayTag WidgetTag;

protected:

	UFUNCTION()
	void HandleModalContentConfirmed(UObject* Payload);

	UFUNCTION()
	void HandleModalContentCancelled();
};
