// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/UI/MounteaBaseWidgetInterface.h"
#include "Interfaces/UI/MounteaInventoryWBPInterface.h"
#include "WBP/MounteaBaseUserWidget.h"
#include "MounteaInventoryBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UMounteaInventoryBaseWidget : public UMounteaBaseUserWidget, public IMounteaInventoryWBPInterface
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

public:

	virtual UMounteaInventoryThemeConfig* GetTheme_Implementation(bool& bResult) const override;

	virtual ESlateVisibility GetInventoryVisibility_Implementation() const override;

	virtual TScriptInterface<IMounteaInventoryInterface> GetOwningInventory_Implementation() const override;
	virtual void SetOwningInventory_Implementation(const TScriptInterface<IMounteaInventoryInterface>& NewOwningInventory) override;
	
	virtual void CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData) override;

protected:

	UPROPERTY(Category="Mountea|Debug", VisibleAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	TScriptInterface<IMounteaInventoryInterface> OwningInventory;

	UPROPERTY(Category="Mountea|Debug", EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	UMounteaInventoryThemeConfig* InventoryTheme;

	UPROPERTY(Category="Mountea|Debug", EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
	ESlateVisibility InventoryVisibility = ESlateVisibility::Hidden;
};
