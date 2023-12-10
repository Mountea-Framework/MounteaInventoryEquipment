// Copyright Dominik Morse 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Helpers/MounteaInventoryHelpers.h"

#include "MounteaInventoryWBPInterface.generated.h"

class UMounteaInventoryThemeConfig;
class IMounteaInventoryItemWBPInterface;
class IMounteaInventoryInterface;
struct FInventoryNotificationData;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UMounteaInventoryWBPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MOUNTEAINVENTORYEQUIPMENT_API IMounteaInventoryWBPInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Mountea|Inventory", meta = (ClassFilter = "MounteaInventoryThemeConfig"), meta=(DeterminesOutputType = "ClassFilter"))//, meta=(CompactNodeTitle="Theme Config", HideSelfPin=true))
	UMounteaInventoryThemeConfig* GetTheme(bool& bResult) const;
	virtual UMounteaInventoryThemeConfig* GetTheme_Implementation(bool& bResult) const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory")
	void CreateInventoryNotification(const FInventoryNotificationData& NotificationData);
	virtual void CreateInventoryNotification_Implementation(const FInventoryNotificationData& NotificationData) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory")
	bool GetInventoryVisibility() const;
	virtual bool GetInventoryVisibility_Implementation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory")
	TScriptInterface<IMounteaInventoryInterface> GetOwningInventory() const;
	virtual TScriptInterface<IMounteaInventoryInterface> GetOwningInventory_Implementation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Mountea|Inventory")
	void SetOwningInventory(const TScriptInterface<IMounteaInventoryInterface>& OwningInventory);
	virtual void SetOwningInventory_Implementation(const TScriptInterface<IMounteaInventoryInterface>& OwningInventory) = 0;
};
