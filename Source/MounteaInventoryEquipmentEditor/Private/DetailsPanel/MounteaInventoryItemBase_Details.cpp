// All rights reserved Dominik Pavlicek 2023


#include "MounteaInventoryItemBase_Details.h"

#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Definitions/MounteaInventoryItem.h"
#include "Helpers/MIEEditorStyle.h"

#include "Widgets/Layout/SScaleBox.h"

#define LOCTEXT_NAMESPACE "FMounteaInventoryItem_Details"

FReply FMounteaInventoryItem_Details::OnDocumentationClicked() const
{
	if (EditingItem)
	{
		FPlatformProcess::LaunchURL(*ItemsWikiPage, nullptr, nullptr);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

const FSlateBrush* FMounteaInventoryItem_Details::GetDocumentationBrush() const
{
	return FMIEEditorStyle::GetBrush( "MIEStyleSet.Buttons.Documentation" );
}

FSlateColor FMounteaInventoryItem_Details::GetDocumentationColorOpacity() const
{
	return FSlateColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
}

void FMounteaInventoryItem_Details::OnDocumentationHovered()
{
	if (!DocumentationButton.IsValid()) return;
	if (!DocumentationImage.IsValid()) return;

	FSlateRenderTransform Unhovered;

	FSlateRenderTransform Hovered = FSlateRenderTransform
	(
		.99f, FVector2D(0.f, 0.f)
	);
	
	DocumentationImage->SetRenderTransform( DocumentationButton->IsHovered() ? Hovered : Unhovered );
}

bool FMounteaInventoryItem_Details::IsSetValidDataEnabled() const
{
	if (EditingItem)
	{
		return EditingItem->ItemDataSource == EItemDataSource::EIDS_SourceTable;
	}

	return false;
}

FReply FMounteaInventoryItem_Details::OnSetValidDataClicked()
{
	if (EditingItem)
	{
		EditingItem->SetValidDataEditor();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

const FSlateBrush* FMounteaInventoryItem_Details::GetSetValidDataBrush() const
{
	return FMIEEditorStyle::GetBrush( "MIEStyleSet.Buttons.Update" );
}

void FMounteaInventoryItem_Details::OnSetValidDataHovered()
{
	if (!ValidDataButton.IsValid()) return;
	if (!ValidDataImage.IsValid()) return;

	FSlateRenderTransform Unhovered;

	FSlateRenderTransform Hovered = FSlateRenderTransform
	(
		.99f, FVector2D(0.f, 0.f)
	);
	
	ValidDataImage->SetRenderTransform( ValidDataButton->IsHovered() ? Hovered : Unhovered );
}

void FMounteaInventoryItem_Details::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

	// Only support one object being customized
	if (ObjectsBeingCustomized.Num() != 1) return;

	const TWeakObjectPtr<UMounteaInventoryItemBase> WeakGraphNode = Cast<UMounteaInventoryItemBase>(ObjectsBeingCustomized[0].Get());
	if (!WeakGraphNode.IsValid()) return;

	EditingItem = WeakGraphNode.Get();
	if (!EditingItem) return;
	
	SavedLayoutBuilder = &DetailBuilder;

	DocumentationButtonStyle = FMIEEditorStyle::GetWidgetStyle<FButtonStyle>(TEXT("MIEStyleSet.Buttons.Style"));
	ValidDataButtonStyle = FAppStyle::Get().GetWidgetStyle<FButtonStyle>("PrimaryButton"); // FMIEEditorStyle::GetWidgetStyle<FButtonStyle>(TEXT("MIEStyleSet.Buttons.Style")); //.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")

	IDetailCategoryBuilder& ItrCategoryBuild = DetailBuilder.EditCategory(TEXT("Documentation"), FText::GetEmpty(), ECategoryPriority::Uncommon);
	ItrCategoryBuild.AddCustomRow(LOCTEXT("MounteaInventoryItem_Details_Documentation", "Documentation"), false)
	.WholeRowWidget
	[
		SNew(SBox)
		.HAlign(HAlign_Fill)
		[
			SNew(SScaleBox)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			.Stretch(EStretch::ScaleToFit)
			[
				SAssignNew(DocumentationButton, SButton)
				.HAlign(HAlign_Fill)
				.Text(LOCTEXT("MounteaInventoryItem_Details_Documentation_Text", "Documentation"))
				.ToolTipText(LOCTEXT("MounteaInventoryItem_Details_Documentation_Tooltip", "Opens a documentation page Mountea Inventory Item."))
				.OnClicked(this, &FMounteaInventoryItem_Details::OnDocumentationClicked)
				.OnHovered(this, &FMounteaInventoryItem_Details::OnDocumentationHovered)
				.OnUnhovered(this, &FMounteaInventoryItem_Details::OnDocumentationHovered)
				.ButtonStyle(&DocumentationButtonStyle)
				[
					SNew(SScaleBox)
					.HAlign(EHorizontalAlignment::HAlign_Left)
					.Stretch(EStretch::ScaleToFit)
					[
						SAssignNew(DocumentationImage, SImage)
					.Image(this, &FMounteaInventoryItem_Details::GetDocumentationBrush)
					]
				]
			]
		]
	];

	const FMargin ContentPadding = FMargin(2.f, 0.f, 2.f,0.f);
	
	IDetailCategoryBuilder& ImportCategoryBuild = DetailBuilder.EditCategory(TEXT("1. Import"), FText::GetEmpty(), ECategoryPriority::Important);
	ImportCategoryBuild.AddCustomRow(LOCTEXT("MounteaInventoryItem_Details_SetValidData", "SetValidData"), false)
	.WholeRowWidget
	[
		SNew(SBox)
		.HAlign(HAlign_Fill)
		.Padding(ContentPadding)
		[

			SNew(SScaleBox)
			.HAlign(HAlign_Left)
			.Stretch(EStretch::ScaleToFit)
			[
				SAssignNew(ValidDataButton, SButton)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Fill)
				.Text(LOCTEXT("MounteaInventoryItem_Details_SetValidData_Text", "Set Valid Data"))
				.ToolTipText(LOCTEXT("MounteaInventoryItem_Details_SetValidData_Tooltip", "Updates Item to match data from Source Table."))
				.OnClicked(this, &FMounteaInventoryItem_Details::OnSetValidDataClicked)
				.OnHovered(this, &FMounteaInventoryItem_Details::OnSetValidDataHovered)
				.OnUnhovered(this, &FMounteaInventoryItem_Details::OnSetValidDataHovered)
				.IsEnabled(this, &FMounteaInventoryItem_Details::IsSetValidDataEnabled)
				.ButtonStyle(&ValidDataButtonStyle)
				[
				
						SNew(SHorizontalBox)
						
						+ SHorizontalBox::Slot()
						.FillWidth(0.1f)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Center)
						.Padding(FMargin(0.f, 0.f, 15.f, 0.f))
						[
							SNew(SScaleBox)
							.HAlign(HAlign_Fill)
							.Stretch(EStretch::ScaleToFit)
							[
								SAssignNew(ValidDataImage, SImage)
								.Image(this, &FMounteaInventoryItem_Details::GetSetValidDataBrush)
								.DesiredSizeOverride(FVector2d(20.f))
							]
						]
					
						+ SHorizontalBox::Slot()
						.FillWidth(0.8f)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Center)
						.Padding(ContentPadding)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("MounteaInventoryItem_Details_SetValidData_Text", "Set Valid Data"))
							.Justification(ETextJustify::Left)
							
						]

				]
			]
		]
	];
}

#undef LOCTEXT_NAMESPACE