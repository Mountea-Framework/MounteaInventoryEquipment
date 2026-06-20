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

#include "Statics/MounteaInventoryWebPageStatics.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Settings/MounteaAdvancedInventorySettingsEditor.h"

FString MounteaInventoryWebPageStatics::InjectSharedAssets(const FString& HtmlContent)
{
	const UMounteaAdvancedInventorySettingsEditor* editorSettings = GetDefault<UMounteaAdvancedInventorySettingsEditor>();
	if (!editorSettings)
		return HtmlContent;

	FString result = HtmlContent;

	if (!editorSettings->SharedStylesheetPath.FilePath.IsEmpty())
	{
		FString cssContent;
		const FString cssPath = FPaths::ConvertRelativePathToFull(editorSettings->SharedStylesheetPath.FilePath);

		if (FFileHelper::LoadFileToString(cssContent, *cssPath))
		{
			const FString styleTag = FString::Printf(TEXT("<style>%s</style>"), *cssContent);
			if (!result.Contains(styleTag))
			{
				const int32 headEndPos = result.Find(TEXT("</head>"), ESearchCase::IgnoreCase);
				if (headEndPos != INDEX_NONE)
					result.InsertAt(headEndPos, styleTag);
				else
					result = styleTag + result;
			}
		}
	}

	if (!editorSettings->SharedScriptPath.FilePath.IsEmpty())
	{
		FString scriptContent;
		const FString scriptPath = FPaths::ConvertRelativePathToFull(editorSettings->SharedScriptPath.FilePath);

		if (FFileHelper::LoadFileToString(scriptContent, *scriptPath))
		{
			const FString scriptTag = FString::Printf(TEXT("<script>%s</script>"), *scriptContent);
			if (!result.Contains(scriptTag))
			{
				const int32 bodyEndPos = result.Find(TEXT("</body>"), ESearchCase::IgnoreCase);
				if (bodyEndPos != INDEX_NONE)
					result.InsertAt(bodyEndPos, scriptTag);
				else
					result += scriptTag;
			}
		}
	}

	return result;
}

FString MounteaInventoryWebPageStatics::GetPluginResourcePath(const FString& RelativePath)
{
	const TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin(TEXT("MounteaAdvancedInventorySystem"));
	if (!plugin.IsValid())
		return FString();

	return FPaths::ConvertRelativePathToFull(FPaths::Combine(plugin->GetBaseDir(), RelativePath));
}

FString MounteaInventoryWebPageStatics::MakeFileBaseUrl(const FString& FilePath)
{
	if (FilePath.IsEmpty())
		return TEXT("about:blank");

	return FString::Printf(TEXT("file:///%s/"), *FPaths::GetPath(FilePath).Replace(TEXT("\\"), TEXT("/")));
}
