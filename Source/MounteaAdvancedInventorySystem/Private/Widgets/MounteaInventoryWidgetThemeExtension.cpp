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

#include "Widgets/MounteaInventoryWidgetThemeExtension.h"

#include "Statics/MounteaInventoryBaseUIStatics.h"
#include "Settings/MounteaAdvancedInventoryThemeConfig.h"

void UMounteaInventoryWidgetThemeExtension::Construct()
{
	if (!this->Initialized && IsValid(this->Theme))
		UMounteaInventoryBaseUIStatics::ApplyWidgetThemeByPrefix(this->GetUserWidget(), this->Theme);
}
