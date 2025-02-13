// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Inventory/MounteaAdvancedInventoryUIInterface.h"
#include "MounteaInventoryUIComponent.generated.h"

/**
 *  Mountea Inventory UI Component
 * 
 */
UCLASS(ClassGroup=(Mountea), Blueprintable,  AutoExpandCategories=("Mountea","Inventory","Mountea|Inventory"), HideCategories=("Cooking","Collision"), meta=(BlueprintSpawnableComponent, DisplayName="Mountea Inventory UI Component"))
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryUIComponent : public UActorComponent, public IMounteaAdvancedInventoryUIInterface
{
	GENERATED_BODY()

public:

	UMounteaInventoryUIComponent();

protected:

	virtual void BeginPlay() override;

	// --- Interface Functions ------------------------------
public:
	virtual TScriptInterface<IMounteaAdvancedInventoryInterface> GetParentInventory_Implementation() const override;
	virtual void SetParentInventory_Implementation(const TScriptInterface<IMounteaAdvancedInventoryInterface>& NewParentInventory) override;
	virtual bool CreateInventoryUI_Implementation() override;
	virtual void RemoveInventoryUI_Implementation() override;
	virtual void SetInventoryUIVisibility_Implementation(const ESlateVisibility NewVisibility) override;
	virtual UUserWidget* GetNotificationContainer_Implementation() const override;
	virtual void SetNotificationContainer_Implementation(UUserWidget* NewNotificationContainer) override;
	virtual void CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData) override;
	virtual void RemoveInventoryNotifications_Implementation() override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory", meta=(AllowPrivateAccess), meta=(ExposeOnSpawn))
	TScriptInterface<IMounteaAdvancedInventoryInterface> ParentInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory", meta=(AllowPrivateAccess))
	TObjectPtr<UUserWidget> InventoryWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mountea|Inventory", meta=(AllowPrivateAccess))
	TObjectPtr<UUserWidget> InventoryNotificationContainerWidget;
};
