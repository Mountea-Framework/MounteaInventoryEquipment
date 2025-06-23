// All rights reserved Dominik Morse 2024


#include "Subsystems/UMounteaInventoryTemplateEditorSubsystem.h"

#include "Definitions/MounteaInventoryItemTemplate.h"

UMounteaInventoryItemTemplate* UUMounteaInventoryTemplateEditorSubsystem::GetOrCreateTempTemplate()
{
	if (!IsValid(TempTemplate))
	{
		TempTemplate = NewObject<UMounteaInventoryItemTemplate>(this);
		TempTemplate->SetFlags(RF_Standalone);
		
		TempTemplate->Guid = FGuid::NewGuid();
		TempTemplate->DisplayName = FText::FromString(TEXT("New Template"));
		TempTemplate->MaxQuantity = 1;
		TempTemplate->MaxStackSize = 1;
		TempTemplate->ItemCategory = TEXT("Default");
		TempTemplate->ItemRarity = TEXT("Common");
	}
	
	return TempTemplate;
}

void UUMounteaInventoryTemplateEditorSubsystem::ClearTempTemplate()
{
	if (IsValid(TempTemplate))
	{
		TempTemplate->MarkAsGarbage();
		TempTemplate = nullptr;
	}
}

bool UUMounteaInventoryTemplateEditorSubsystem::HasTempTemplate() const
{ return IsValid(TempTemplate); }
