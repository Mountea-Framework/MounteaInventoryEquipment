// All rights reserved Dominik Morse 2024


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
