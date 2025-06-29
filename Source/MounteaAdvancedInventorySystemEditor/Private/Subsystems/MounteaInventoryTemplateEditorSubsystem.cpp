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


#include "Subsystems/MounteaInventoryTemplateEditorSubsystem.h"

#include "Definitions/MounteaInventoryItemTemplate.h"

UMounteaInventoryItemTemplate* UMounteaInventoryTemplateEditorSubsystem::GetOrCreateTempTemplate()
{
	if (!IsValid(TempTemplate))
	{
		TempTemplate = NewObject<UMounteaInventoryItemTemplate>(this);
		TempTemplate->SetFlags(RF_Transient  | RF_Standalone);
		
		TempTemplate->Guid = FGuid::NewGuid();
		TempTemplate->DisplayName = FText::FromString(TEXT("New Template"));
		TempTemplate->MaxQuantity = 1;
		TempTemplate->MaxStackSize = 1;
		TempTemplate->ItemCategory = TEXT("Miscellaneous");
		TempTemplate->ItemRarity = TEXT("Common");
	}
	
	return TempTemplate;
}

void UMounteaInventoryTemplateEditorSubsystem::ClearTempTemplate()
{
	if (IsValid(TempTemplate))
	{
		TempTemplate->MarkAsGarbage();
		TempTemplate = nullptr;
	}
}

bool UMounteaInventoryTemplateEditorSubsystem::HasTempTemplate() const
{ return IsValid(TempTemplate); }
