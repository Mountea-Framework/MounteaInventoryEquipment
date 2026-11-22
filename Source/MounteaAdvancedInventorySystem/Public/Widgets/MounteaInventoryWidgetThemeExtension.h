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
#include "Extensions/UserWidgetExtension.h"
#include "MounteaInventoryWidgetThemeExtension.generated.h"

class UMounteaAdvancedInventoryThemeConfig;

/** Per-instance theme ownership survives Slate reconstruction and keeps the resolved asset alive. */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEM_API UMounteaInventoryWidgetThemeExtension : public UUserWidgetExtension
{
	GENERATED_BODY()

public:
	virtual void Construct() override;

	UPROPERTY(Transient)
	TObjectPtr<UMounteaAdvancedInventoryThemeConfig> Theme;

	bool Initialized = false;
};
