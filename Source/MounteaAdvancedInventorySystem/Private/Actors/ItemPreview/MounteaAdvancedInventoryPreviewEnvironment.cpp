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

#include "Actors/ItemPreview/MounteaAdvancedInventoryPreviewEnvironment.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"


AMounteaAdvancedInventoryPreviewEnvironment::AMounteaAdvancedInventoryPreviewEnvironment()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootSceneComponent;
        
	DirectionalLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Sun"));
	DirectionalLight->SetupAttachment(RootComponent);
	DirectionalLight->Intensity = 3.0f;
        
	SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("Sky"));
	SkyLight->SetupAttachment(RootComponent);
	SkyLight->Intensity = 1.0f;
}

