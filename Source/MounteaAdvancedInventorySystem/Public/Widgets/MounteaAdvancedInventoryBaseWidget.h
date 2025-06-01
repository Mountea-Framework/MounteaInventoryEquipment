// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/Widgets/MounteaInventoryGenericWidgetInterface.h"
#include "Interfaces/Widgets/BaseWidget/MounteaAdvancedBaseInventoryWidgetInterface.h"
#include "MounteaAdvancedInventoryBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryBaseWidget : public UUserWidget,
public IMounteaInventoryGenericWidgetInterface,
public IMounteaAdvancedBaseInventoryWidgetInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	virtual void ProcessInventoryWidgetCommand_Implementation(const FString& Command, UObject* OptionalPayload = nullptr) override {};
	virtual void ApplyTheme_Implementation() override {};
	virtual void RefreshWidget_Implementation() override {};

	virtual void SetOwningInventoryUI_Implementation(const TScriptInterface<IMounteaAdvancedInventoryUIInterface>& NewOwningInventoryUI) override
	{
		if (ParentUIComponent != NewOwningInventoryUI)
			ParentUIComponent = NewOwningInventoryUI;
	};

protected:
	
	/** Reference to the owning inventory UI interface.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Primary Data", meta=(AllowPrivateAccess))
	TScriptInterface<IMounteaAdvancedInventoryUIInterface> ParentUIComponent = nullptr;
};
