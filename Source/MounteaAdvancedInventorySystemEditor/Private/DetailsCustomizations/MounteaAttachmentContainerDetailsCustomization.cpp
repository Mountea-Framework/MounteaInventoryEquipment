// All rights reserved Dominik Morse 2024


#include "MounteaAttachmentContainerDetailsCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Components/MounteaAttachmentContainerComponent.h"

void FMounteaAttachmentContainerDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// Get the selected component object
	TArray<TWeakObjectPtr<UObject>> Objs;
	DetailBuilder.GetObjectsBeingCustomized(Objs);
	if (Objs.Num() != 1) return;

	if (auto* Comp = Cast<UMounteaAttachmentContainerComponent>(Objs[0].Get()))
	{
		// Collect names
		TArray<TSharedPtr<FName>> Options;
		AActor* Owner = Comp->GetOwner();
		if (Owner)
		{
			TArray<USceneComponent*> Comps;
			Owner->GetComponents<USceneComponent>(Comps);
			for (USceneComponent* SC : Comps)
				Options.Add(MakeShareable(new FName(SC->GetFName())));
		}

		// Build the property handle for TargetName
		TSharedRef<IPropertyHandle> Prop = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UMounteaAttachmentContainerComponent, DefaultAttachmentTarget));
		IDetailCategoryBuilder& Cat = DetailBuilder.EditCategory("Settings");

		// Replace it with a combo box
		Cat.AddProperty(Prop)
		.CustomWidget()
		.NameContent()
		[
			Prop->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MaxDesiredWidth(300)
		[
			SNew(SComboBox<TSharedPtr<FName>>)
			.OptionsSource(&Options)
			.OnGenerateWidget_Lambda([](TSharedPtr<FName> In){ return SNew(STextBlock).Text(FText::FromName(*In)); })
			.OnSelectionChanged_Lambda([Prop](TSharedPtr<FName> Sel, ESelectInfo::Type){
				if (Sel.IsValid())
					Prop->SetValue(*Sel);
			})
			.Content()
			[
				SNew(STextBlock)
				.Text_Lambda([Prop](){
					FName Current;
					Prop->GetValue(Current);
					return FText::FromName(Current);
				})
			]
		];
	}
}
