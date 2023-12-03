// All rights reserved Dominik Morse 2023


#include "MounteaEquipmentConfig_Details.h"


#include "DetailLayoutBuilder.h"

#include "GameplayTagsManager.h"

#include "Settings/MounteaEquipmentConfigData.h"

#define LOCTEXT_NAMESPACE "FMounteaEquipmentConfig_Details"


FMounteaEquipmentConfig_Details::~FMounteaEquipmentConfig_Details()
{
	if (Editing)
	{
		Editing->OnTagModified.Unbind();
	}
}

void FMounteaEquipmentConfig_Details::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

	// Only support one object being customized
	if (ObjectsBeingCustomized.Num() != 1) return;

	const TWeakObjectPtr<UMounteaEquipmentConfigData> WeakGraphNode = Cast<UMounteaEquipmentConfigData>(ObjectsBeingCustomized[0].Get());
	if (!WeakGraphNode.IsValid()) return;

	Editing = WeakGraphNode.Get();
	if (!Editing) return;
	
	SavedLayoutBuilder = &DetailBuilder;

	TArray< TSharedPtr<FStructOnScope> > OutStructs;
	DetailBuilder.GetStructsBeingCustomized(OutStructs);
	
	Editing->OnTagModified.BindRaw(this, &FMounteaEquipmentConfig_Details::OnTagModified);

}

void FMounteaEquipmentConfig_Details::OnTagModified(FGameplayTag& ModifiedTag)
{	
	if (auto Container = UGameplayTagsManager::Get().GetSingleTagContainer(ModifiedTag))
	{
		FGameplayTagContainer NewContainer = *Container;
		NewContainer.Reset();

		NewContainer = FGameplayTagContainer::EmptyContainer;

		FGameplayTag EmptyTag;
		NewContainer.AddTagFast(EmptyTag);

		Container = &NewContainer;
		
		ModifiedTag = EmptyTag;
		
		auto SlotTagProperty = SavedLayoutBuilder->GetProperty(GET_MEMBER_NAME_CHECKED(FMounteaEquipmentSlotIdentity, SlotTag));

		if (SlotTagProperty.Get().IsValidHandle())
		{
			SlotTagProperty->ResetToDefault();
		}
	}
}

#undef LOCTEXT_NAMESPACE
