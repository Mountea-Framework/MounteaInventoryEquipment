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


#include "MounteaAttachmentContainerDetailsCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Widgets/Input/SComboBox.h"

#include "Components/MounteaAttachmentContainerComponent.h"
#include "Logs/MounteaAdvancedInventoryLog.h"

void FMounteaAttachmentContainerDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objs;
	DetailBuilder.GetObjectsBeingCustomized(Objs);
	if (Objs.Num() != 1)
		return;
	UMounteaAttachmentContainerComponent* Comp = Cast<UMounteaAttachmentContainerComponent>(Objs[0].Get());
	if (!Comp)
		return;

	// find an actor or CDO to pull components from
	AActor* ownerActor = Comp->GetOwner();
	if (!IsValid(ownerActor))
	{
		if (UBlueprintGeneratedClass* bpClass = Cast<UBlueprintGeneratedClass>(Comp->GetOuter()))
		{
			ownerActor = Cast<AActor>(bpClass->GetDefaultObject());
		}
	}
		
	if (!IsValid(ownerActor))
	{
		//LOG_WARNING(TEXT("[%s] Could not find owner actor or class default"), *Comp->GetName());
	}

	TArray<TSharedPtr<FName>> Options;
	if (IsValid(ownerActor))
	{
		TArray<USceneComponent*> sceneComponents;
		ownerActor->GetComponents<USceneComponent>(sceneComponents);
		for (USceneComponent* sceneComponent : sceneComponents)
		{
			if (IsValid(sceneComponent))
				Options.Add(MakeShareable(new FName(sceneComponent->GetFName())));
		}
	}

	TSharedRef<IPropertyHandle> Prop = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UMounteaAttachmentContainerComponent, DefaultAttachmentTarget));
	IDetailCategoryBuilder& Cat = DetailBuilder.EditCategory("Attachment");

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
		.OnGenerateWidget_Lambda([](TSharedPtr<FName> In)
		{
			return SNew(STextBlock).Text(FText::FromName(*In));
		})
		.OnSelectionChanged_Lambda([Prop](TSharedPtr<FName> Sel, ESelectInfo::Type)
		{
			if (Sel.IsValid())
				Prop->SetValue(*Sel);
		})
		.Content()
		[
			SNew(STextBlock)
			.Text_Lambda([Prop]()
			{
				FName Current;
				Prop->GetValue(Current);
				return FText::FromName(Current);
			})
		]
	];
}

