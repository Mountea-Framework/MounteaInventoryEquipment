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
#include "Engine/DataAsset.h"
#include "MounteaAdvancedInventoryScreenUIConfig.generated.h"

class UMounteaAdvancedInventoryThemeConfig;

/** Shared screen settings; an empty override inherits the global UI theme. */
UCLASS(Abstract, ClassGroup=(Mountea), BlueprintType)
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaAdvancedInventoryScreenUIConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Complete replacement theme for screens opened with this config instance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Theme")
	TSoftObjectPtr<UMounteaAdvancedInventoryThemeConfig> ThemeOverride;
#if WITH_EDITOR
public:
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
