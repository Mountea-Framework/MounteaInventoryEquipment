// All rights reserved Dominik Pavlicek 2022.


#include "Definitions/MounteaItemAdditionalData.h"

UMounteaItemAdditionalData::UMounteaItemAdditionalData()
{
	if (const FMounteaItemDescription* Row = GetRowData(SourceTable_ShortDescription, SourceRow_ShortDescription))
	{
		ItemShortDescription = Row->ItemShortDescription;
	}

	if (const FMounteaItemDescription* Row = GetRowData(SourceTable_LongDescription, SourceRow_LongDescription))
	{
		ItemLongDescription = Row->ItemLongDescription;
	}
}

FMounteaItemDescription* UMounteaItemAdditionalData::GetRowData(const UDataTable* FromTable, const FName FromRow)
{
	if (!FromTable) return nullptr;
	
	const FString Context;
	return FromTable->FindRow<FMounteaItemDescription>(FromRow, Context);
}

#if WITH_EDITOR

void UMounteaItemAdditionalData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaItemAdditionalData, SourceTable_ShortDescription))
	{
		SourceRow_ShortDescription = FName();
		ItemShortDescription = FText();
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaItemAdditionalData, SourceRow_ShortDescription))
	{
		if (SourceTable_ShortDescription)
		{
			if (const FMounteaItemDescription* Row = GetRowData(SourceTable_ShortDescription, SourceRow_ShortDescription))
			{
				ItemShortDescription = Row->ItemShortDescription;
			}
		}
	}
	
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaItemAdditionalData, SourceTable_LongDescription))
	{
		SourceRow_LongDescription = FName();
		ItemLongDescription = FText();
	}

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMounteaItemAdditionalData, SourceRow_LongDescription))
	{
		if (SourceTable_LongDescription)
		{
			if (const FMounteaItemDescription* Row = GetRowData(SourceTable_LongDescription, SourceRow_LongDescription))
			{
				ItemLongDescription = Row->ItemLongDescription;
			}
		}
	}
}
#endif
