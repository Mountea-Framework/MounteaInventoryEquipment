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
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventoryUIConfig.generated.h"

/**
 * UMounteaAdvancedInventoryUIConfig is a data asset managing comprehensive inventory system UI configuration.
 * Settings config defines visual parameters for complete inventory system customization and behavior control.
 *
 * @see [Settings Configuration](https://mountea.tools/docs/AdvancedInventoryEquipmentSystem/Settings)
 */
UCLASS(ClassGroup=(Mountea), BlueprintType, Blueprintable, DisplayName="Inventory Settings UI Config", meta=(ShortTooltip="Configuration asset for the Mountea Inventory System defining visual components."))

class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryUIConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	// --- User Interface: Wrapper

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Wrapper",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventorySystemWrapperWidgetInterface"))
	TSoftClassPtr<UUserWidget> UserInterfaceWrapperClass;
	
	// ---- Notifications
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Notifications|Classes",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryNotificationContainerWidgetInterface"))
	TSoftClassPtr<UUserWidget> NotificationNotificationWidgetContainerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Notifications|Classes",
		meta=(MustImplement="/Script/MounteaAdvancedInventorySystem.MounteaInventoryNotificationWidgetInterface"))
	TSoftClassPtr<UUserWidget> NotificationWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Notifications|Notification Card")
	TSoftObjectPtr<UMaterialInterface> NotificationCardMaterial = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="✨ UserInterface|Notifications|Notification Card")
	FVector2f NotificationCardSize = FVector2f(150.f, 60.f);
	
	// ---- 
};
