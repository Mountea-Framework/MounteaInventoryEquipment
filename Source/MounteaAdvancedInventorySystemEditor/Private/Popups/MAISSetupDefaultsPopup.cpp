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

#include "Popups/MAISSetupDefaultsPopup.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/FileHelper.h"
#include "Setup/MounteaInventorySetupUtilities.h"
#include "Slate/MounteaInventoryWebBrowserWrapper.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Statics/MounteaInventoryWebPageStatics.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SWindow.h"

#define LOCTEXT_NAMESPACE "MAISSetupDefaultsPopup"

namespace MAISSetupDefaultsPopupInternal
{
	FString StatusToText(const EMAISSetupItemStatus Status)
	{
		switch (Status)
		{
			case EMAISSetupItemStatus::AlreadyPresent:	return TEXT("Already Present");
			case EMAISSetupItemStatus::Added:			return TEXT("Added");
			case EMAISSetupItemStatus::NativeClass:		return TEXT("Native Class");
			case EMAISSetupItemStatus::Failed:			return TEXT("Failed");
			case EMAISSetupItemStatus::Skipped:			return TEXT("Skipped");
			default:									return TEXT("Unknown");
		}
	}

	int32 CountStatus(const FMAISSetupDefaultsReport& Report, const EMAISSetupItemStatus Status)
	{
		int32 count = 0;

		for (const FMAISSetupItemResult& result : Report.SettingsResults)
		{
			if (result.Status == Status)
				++count;
		}

		for (const FMAISSetupItemResult& result : Report.PlayerResults)
		{
			if (result.Status == Status)
				++count;
		}

		return count;
	}

	TArray<TSharedPtr<FJsonValue>> MakeResultArray(const TArray<FMAISSetupItemResult>& Results)
	{
		TArray<TSharedPtr<FJsonValue>> jsonResults;

		for (const FMAISSetupItemResult& result : Results)
		{
			TSharedPtr<FJsonObject> jsonResult = MakeShared<FJsonObject>();
			jsonResult->SetStringField(TEXT("itemName"), result.ItemName);
			jsonResult->SetStringField(TEXT("status"), StatusToText(result.Status));
			jsonResult->SetStringField(TEXT("message"), result.Message);
			jsonResults.Add(MakeShared<FJsonValueObject>(jsonResult));
		}

		return jsonResults;
	}

	FString BuildReportJson(const FMAISSetupDefaultsReport& Report)
	{
		const int32 addedCount = CountStatus(Report, EMAISSetupItemStatus::Added);
		const int32 presentCount = CountStatus(Report, EMAISSetupItemStatus::AlreadyPresent);
		const int32 warningCount = CountStatus(Report, EMAISSetupItemStatus::NativeClass);
		const int32 failedCount = CountStatus(Report, EMAISSetupItemStatus::Failed);
		const FString resultText = failedCount > 0 ? TEXT("Setup completed with failures") : warningCount > 0 ? TEXT("Setup completed with warnings") : TEXT("Setup completed successfully");
		const FString gameModeText = Report.GameModeName.IsEmpty() ? TEXT("Not resolved") : Report.GameModeName;
		const FString playerText = Report.PlayerClassName.IsEmpty() ? TEXT("Not resolved") : Report.PlayerClassName;
		const FString gameModeSource = Report.bIsDefaultGameMode ? TEXT("Project default GameMode fallback") : TEXT("World GameMode");

		TSharedPtr<FJsonObject> jsonReport = MakeShared<FJsonObject>();
		jsonReport->SetStringField(TEXT("summary"), resultText);

		TSharedPtr<FJsonObject> metrics = MakeShared<FJsonObject>();
		metrics->SetNumberField(TEXT("added"), addedCount);
		metrics->SetNumberField(TEXT("alreadyPresent"), presentCount);
		metrics->SetNumberField(TEXT("warnings"), warningCount);
		metrics->SetNumberField(TEXT("failed"), failedCount);
		jsonReport->SetObjectField(TEXT("metrics"), metrics);

		TSharedPtr<FJsonObject> context = MakeShared<FJsonObject>();
		context->SetStringField(TEXT("gameMode"), gameModeText);
		context->SetStringField(TEXT("gameModeSource"), gameModeSource);
		context->SetStringField(TEXT("playerClass"), playerText);
		context->SetBoolField(TEXT("settingsSaved"), Report.bSettingsSaved);
		jsonReport->SetObjectField(TEXT("context"), context);

		jsonReport->SetArrayField(TEXT("settingsResults"), MakeResultArray(Report.SettingsResults));
		jsonReport->SetArrayField(TEXT("playerResults"), MakeResultArray(Report.PlayerResults));

		FString jsonString;
		const TSharedRef<TJsonWriter<>> writer = TJsonWriterFactory<>::Create(&jsonString);
		FJsonSerializer::Serialize(jsonReport.ToSharedRef(), writer);
		jsonString.ReplaceInline(TEXT("</"), TEXT("<\\/"));
		return jsonString;
	}

	FString GetReportTemplatePath()
	{
		return MounteaInventoryWebPageStatics::GetPluginResourcePath(TEXT("Resources/Help/MAISSetupDefaultsReport.html"));
	}

	FString BuildHtml(const FMAISSetupDefaultsReport& Report)
	{
		FString htmlContent;
		const FString templatePath = GetReportTemplatePath();
		if (!templatePath.IsEmpty() && FFileHelper::LoadFileToString(htmlContent, *templatePath))
		{
			htmlContent.ReplaceInline(TEXT("__MAIS_SETUP_REPORT_JSON__"), *BuildReportJson(Report));
			return MounteaInventoryWebPageStatics::InjectSharedAssets(htmlContent);
		}

		return FString();
	}
}

void MAISSetupDefaultsPopup::Open(const FMAISSetupDefaultsReport& Report)
{
	TSharedPtr<SMounteaInventoryWebBrowserWrapper> webBrowser;

	const TSharedRef<SWindow> window = SNew(SWindow)
		.Title(LOCTEXT("Title", "MAIS Setup Defaults"))
		.ClientSize(FVector2D(980.f, 720.f))
		.SupportsMaximize(true)
		.SupportsMinimize(false)
		[
			SNew(SBox)
			.Padding(0)
			[
				SAssignNew(webBrowser, SMounteaInventoryWebBrowserWrapper)
				.InitialURL(TEXT("about:blank"))
			]
		];

	FSlateApplication::Get().AddWindow(window);

	if (webBrowser.IsValid())
	{
		const FString templatePath = MAISSetupDefaultsPopupInternal::GetReportTemplatePath();
		const FString baseUrl = templatePath.IsEmpty()
			? TEXT("file:///MounteaAdvancedInventorySetupDefaults/")
			: MounteaInventoryWebPageStatics::MakeFileBaseUrl(templatePath);

		webBrowser->LoadString(
			MAISSetupDefaultsPopupInternal::BuildHtml(Report),
			baseUrl
		);
	}
}

#undef LOCTEXT_NAMESPACE
