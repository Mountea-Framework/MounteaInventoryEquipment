// All rights reserved Dominik Pavlicek 2023


#include "MounteaSettings_Details.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "GameplayTagsManager.h"
#include "HttpModule.h"
#include "Helpers/FMounteaInventoryEquipmentEditorConsts.h"
#include "Helpers/MIEEditorStyle.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IPluginManager.h"
#include "Settings/MounteaInventoryEquipmentSettings.h"
#include "Widgets/Images/SThrobber.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

#define LOCTEXT_NAMESPACE "FMounteaSettings_Details"


FMounteaSettings_Details::~FMounteaSettings_Details()
{
	if (Request.Get())
	{
		Request->CancelRequest();
		Request->OnProcessRequestComplete().Unbind();
	}
}

void FMounteaSettings_Details::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

	// Only support one object being customized
	if (ObjectsBeingCustomized.Num() != 1) return;

	const TWeakObjectPtr<UMounteaInventoryEquipmentSettings> WeakGraphNode = Cast<UMounteaInventoryEquipmentSettings>(ObjectsBeingCustomized[0].Get());
	if (!WeakGraphNode.IsValid()) return;

	Editing = WeakGraphNode.Get();
	if (!Editing) return;
	
	SavedLayoutBuilder = &DetailBuilder;

	RefreshTagsButtonStyle = FMIEEditorStyle::GetWidgetStyle<FButtonStyle>(TEXT("MIEStyleSet.Buttons.Style"));

	const FMargin ContentPadding = FMargin(2.f, 0.f, 2.f,0.f);
	
	IDetailCategoryBuilder& ImportCategoryBuild = DetailBuilder.EditCategory(TEXT("0. Import"), FText::GetEmpty(), ECategoryPriority::Important);
	ImportCategoryBuild.AddCustomRow(LOCTEXT("MounteaInventoryItem_Details_RefreshTags", "RefreshTags"), false)
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
				SAssignNew(RefreshTagsButton, SButton)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Fill)
				.Text(LOCTEXT("MounteaInventoryItem_Details_RefreshTags_Text", "Refresh Tags"))
				.ToolTipText(LOCTEXT("MounteaInventoryItem_Details_RefreshTags_Tooltip", "Updates gameplay Tags for `Mountea Inventory & Equipment` plugin from GitHub repository"))
				.OnClicked(this, &FMounteaSettings_Details::OnRefreshTagsClicked)
				.OnHovered(this, &FMounteaSettings_Details::OnRefreshTagsHovered)
				.OnUnhovered(this, &FMounteaSettings_Details::OnRefreshTagsHovered)
				.IsEnabled(true)
				.ButtonStyle(&RefreshTagsButtonStyle)
				[
					SNew(SHorizontalBox)
					
					+ SHorizontalBox::Slot()
					.FillWidth(0.1f)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					.Padding(FMargin(0.f, 0.f, 15.f, 0.f))
					[
						SAssignNew(RefreshTagsWidgetSwitcher, SWidgetSwitcher)
						.WidgetIndex(this, &FMounteaSettings_Details::GetWidgetSwitcherIndex)

						//0
						+SWidgetSwitcher::Slot()
						[
							SNew(SScaleBox)
							.HAlign(HAlign_Fill)
							.Stretch(EStretch::ScaleToFit)
							[
								SAssignNew(RefreshTagsImage, SImage)
								.Image(this, &FMounteaSettings_Details::GetRefreshTagsBrush)
								.DesiredSizeOverride(FVector2d(20.f))
							]
						]

						//1
						+SWidgetSwitcher::Slot()
						[
							SNew(SScaleBox)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Bottom)
							.Stretch(EStretch::ScaleToFit)
							[
								SNew(SThrobber)
								.Animate(SThrobber::EAnimation::Horizontal)
								.NumPieces(8)
							]
						]
					]
				
					+ SHorizontalBox::Slot()
					.FillWidth(0.8f)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Center)
					.Padding(ContentPadding)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("MounteaInventoryItem_Details_RefreshTags_Text", "Refresh Tags"))
						.Justification(ETextJustify::Left)
						
					]
				]
			]
		]
	];
}

void FMounteaSettings_Details::OnGetResponse_Tags(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg)
{
	FString ResponseBody;
	if (HttpResponse.Get() == nullptr) return;
	
	if (HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
	{
		ResponseBody = HttpResponse->GetContentAsString();
	}
	
	if (!DoesHaveValidTags())
	{
		CreateTagsConfig(ResponseBody);
	}
	else
	{
		UpdateTagsConfig(ResponseBody);
	}

	WidgetSwitcherIndex = 0;
}

FReply FMounteaSettings_Details::OnRefreshTagsClicked()
{
	WidgetSwitcherIndex = 1;
	
	Http = &FHttpModule::Get();

	Request = Http->CreateRequest();
	
	Request->OnProcessRequestComplete().BindRaw(this, &FMounteaSettings_Details::OnGetResponse_Tags);
	Request->SetURL(GameplayTagsURL);

	Request->SetVerb("GET");
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "text");
	Request->ProcessRequest();

	return FReply::Handled();
}

const FSlateBrush* FMounteaSettings_Details::GetRefreshTagsBrush() const
{
	return FMIEEditorStyle::GetBrush( "MIEStyleSet.Buttons.Update" );
}

FSlateColor FMounteaSettings_Details::GetRefreshTagsColorOpacity() const
{
	return FSlateColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
}

int32 FMounteaSettings_Details::GetWidgetSwitcherIndex() const
{
	return WidgetSwitcherIndex;
}

void FMounteaSettings_Details::OnRefreshTagsHovered()
{
	if (!RefreshTagsButton.IsValid()) return;
	if (!RefreshTagsImage.IsValid()) return;

	FSlateRenderTransform Unhovered;

	FSlateRenderTransform Hovered = FSlateRenderTransform
	(
		.99f, FVector2D(0.f, 0.f)
	);
	
	RefreshTagsImage->SetRenderTransform( RefreshTagsButton->IsHovered() ? Hovered : Unhovered );
}

void FMounteaSettings_Details::RefreshGameplayTags() const
{
	TSharedPtr<IPlugin> ThisPlugin = IPluginManager::Get().FindPlugin(TEXT("MounteaInventoryEquipment"));
	check(ThisPlugin.IsValid());
	
	UGameplayTagsManager::Get().EditorRefreshGameplayTagTree();
}

void FMounteaSettings_Details::UpdateTagsConfig(const FString& Context) const
{
	if (!GConfig) return;

	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("MounteaInventoryEquipment"))->GetBaseDir();
	const FString ConfigFilePath = PluginDirectory + "/Config/Tags/MounteaInventoryEquipmentTags.ini";

	FConfigFile* CurrentConfig = GConfig->Find(ConfigFilePath);

	FString CurrentContent;
	CurrentConfig->WriteToString(CurrentContent);

	TArray<FString> Lines;
	Context.ParseIntoArray(Lines, TEXT("\n"), true);

	TArray<FString> CleanedLines;
	for (FString& Itr : Lines)
	{
		if (Itr.Equals("[/Script/GameplayTags.GameplayTagsList]")) continue;

		if (Itr.Contains("GameplayTagList="))
		{
			FString NewValue = Itr.Replace(TEXT("GameplayTagList="), TEXT(""));

			CleanedLines.Add(NewValue);
		}
	}

	if (!CurrentContent.Equals(Context))
	{
		TArray<FString> CurrentLines;
		FConfigFile NewConfig;
		NewConfig.SetArray(TEXT("/Script/GameplayTags.GameplayTagsList"), TEXT("GameplayTagList"), CleanedLines);
		CurrentConfig->GetArray(TEXT("/Script/GameplayTags.GameplayTagsList"), TEXT("GameplayTagList"), CurrentLines);

		for (const FString& Itr : CleanedLines)
		{
			if (CurrentLines.Contains(Itr)) continue;

			CurrentLines.AddUnique(Itr);
		}

		CurrentConfig->SetArray(TEXT("/Script/GameplayTags.GameplayTagsList"), TEXT("GameplayTagList"), CurrentLines);
		CurrentConfig->Write(ConfigFilePath);

		RefreshGameplayTags();
	}
}

void FMounteaSettings_Details::CreateTagsConfig(const FString& Context) const
{
	if (!GConfig) return;

	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("MounteaInventoryEquipment"))->GetBaseDir();
	const FString ConfigFilePath = PluginDirectory + "/Config/Tags/MounteaInventoryEquipmentTags.ini";

	TArray<FString> Lines;
	Context.ParseIntoArray(Lines, TEXT("\n"), true);

	TArray<FString> CleanedLines;
	for (FString& Itr : Lines)
	{
		if (Itr.Equals("[/Script/GameplayTags.GameplayTagsList]")) continue;

		if (Itr.Contains("GameplayTagList="))
		{
			FString NewValue = Itr.Replace(TEXT("GameplayTagList="), TEXT(""));

			CleanedLines.Add(NewValue);
		}
	}
	
	FConfigFile NewConfig;
	NewConfig.SetArray(TEXT("/Script/GameplayTags.GameplayTagsList"), TEXT("GameplayTagList"), CleanedLines);
	NewConfig.Write(ConfigFilePath);
}

bool FMounteaSettings_Details::DoesHaveValidTags()
{
	if (!GConfig) return false;
	
	const FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("MounteaInventoryEquipment"))->GetBaseDir();
	const FString ConfigFilePath = PluginDirectory + "/Config/Tags/MounteaInventoryEquipmentTags.ini";

	if (FPaths::FileExists(ConfigFilePath))
	{
		FString ConfigContent;
		FConfigFile* ConfigFile = GConfig->Find(ConfigFilePath);

		return true;
	}
	
	return false;
}

#undef LOCTEXT_NAMESPACE
