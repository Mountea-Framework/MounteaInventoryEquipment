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
#include "Components/PostProcessComponent.h"
#include "Components/SkyLightComponent.h"

#include "Engine/TextureCube.h"

#include "Settings/TemplatesConfig/MounteaAdvancedInventoryPreviewEnvironmentSettings.h"


AMounteaAdvancedInventoryPreviewEnvironment::AMounteaAdvancedInventoryPreviewEnvironment()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootSceneComponent;
}

void AMounteaAdvancedInventoryPreviewEnvironment::InitializeFromSettings(const UMounteaAdvancedInventoryPreviewEnvironmentSettings* Settings)
{
	if (!Settings) return;

	if (Settings->DirectionalLight.bEnabled)
	{
		if (!DirectionalLight)
		{
			DirectionalLight = NewObject<UDirectionalLightComponent>(this, TEXT("DirectionalLight"));
			DirectionalLight->SetupAttachment(RootComponent);
			DirectionalLight->RegisterComponent();
		}
		
		DirectionalLight->SetIntensity(Settings->DirectionalLight.Intensity);
		DirectionalLight->SetLightColor(Settings->DirectionalLight.LightColor);
		DirectionalLight->SetRelativeRotation(Settings->DirectionalLight.Rotation);
		DirectionalLight->SetCastShadows(Settings->DirectionalLight.bCastShadows);
	}

	if (Settings->SkyLight.bEnabled)
	{
		if (!SkyLight)
		{
			SkyLight = NewObject<USkyLightComponent>(this, TEXT("SkyLight"));
			SkyLight->SetupAttachment(RootComponent);
			SkyLight->RegisterComponent();
		}
		
		SkyLight->SetIntensity(Settings->SkyLight.Intensity);
		
		if (Settings->SkyLight.Cubemap.IsValid())
		{
			SkyLight->SetCubemap(Settings->SkyLight.Cubemap.LoadSynchronous());
			SkyLight->SourceType = ESkyLightSourceType::SLS_SpecifiedCubemap;
		}
		
		SkyLight->bRealTimeCapture = false;
	}

	if (Settings->PostProcess.bEnabled)
	{
		if (!PostProcessComponent)
		{
			PostProcessComponent = NewObject<UPostProcessComponent>(this, TEXT("PostProcess"));
			PostProcessComponent->SetupAttachment(RootComponent);
			PostProcessComponent->RegisterComponent();
		}
		
		PostProcessComponent->Settings = Settings->PostProcess.PostProcessSettings;
		PostProcessComponent->bEnabled = true;
		PostProcessComponent->bUnbound = true;
	}
	
	UWorld* parentWorld = GetWorld();
	if (parentWorld && !Settings->ExtraActorsToSpawn.IsEmpty())
	{
		for (const TSoftClassPtr<AActor>& actorClassPtr : Settings->ExtraActorsToSpawn)
		{
			TSubclassOf<AActor> actorClass = actorClassPtr.LoadSynchronous();
			if (!actorClass)
				continue;
			
			AActor* spawnedActor = parentWorld->SpawnActor<AActor>(actorClass);
			if (!spawnedActor)
				continue;
			
			SpawnedExtraActors.Add(spawnedActor);
		}
	}
}
