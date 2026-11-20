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

#include "Settings/MounteaAdvancedInventoryScreenUIConfig.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#include "Settings/MounteaAdvancedInventoryThemeConfig.h"
#include "Settings/MounteaAdvancedInventoryGlobalUIConfig.h"
#include "Statics/MounteaInventoryBaseUIStatics.h"

EDataValidationResult UMounteaAdvancedInventoryScreenUIConfig::IsDataValid(FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	if (!this->ThemeOverride.IsNull())
	{
		const UMounteaAdvancedInventoryThemeConfig* theme = this->ThemeOverride.LoadSynchronous();

		if (!IsValid(theme))
			Context.AddError(FText::FromString(FString::Printf(TEXT("Cannot resolve screen theme override '%s'."), *this->ThemeOverride.ToSoftObjectPath().ToString())));
		else
		{
			const UMounteaAdvancedInventoryGlobalUIConfig* globalConfig = UMounteaInventoryBaseUIStatics::GetGlobalUIConfig();

			if (IsValid(globalConfig))
				globalConfig->ValidateThemeRules(theme, Context);
		}
	}

	return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif
