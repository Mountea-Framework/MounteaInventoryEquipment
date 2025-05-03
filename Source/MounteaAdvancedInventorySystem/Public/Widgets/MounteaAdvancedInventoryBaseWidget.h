// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/Widgets/MounteaInventoryGenericWidgetInterface.h"
#include "MounteaAdvancedInventoryBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryBaseWidget : public UUserWidget, public IMounteaInventoryGenericWidgetInterface
{
	GENERATED_BODY()

public:

	virtual void ProcessInventoryWidgetCommand_Implementation(const FString& Command, UObject* OptionalPayload = nullptr) override {};
	virtual void ApplyTheme_Implementation() override {};
};
