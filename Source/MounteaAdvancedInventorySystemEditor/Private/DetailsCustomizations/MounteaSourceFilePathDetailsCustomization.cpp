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

#include "MounteaSourceFilePathDetailsCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"

#include "Slate/SMounteaSourceFilePathWidget.h"

void FMounteaSourceFilePathDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	const TSharedRef<IPropertyHandle> sourceFilePathProp = DetailBuilder.GetProperty(TEXT("SourceFilePath"));
	if (!sourceFilePathProp->IsValidHandle())
		return;

	IDetailCategoryBuilder& category = DetailBuilder.EditCategory("Technical Data");

	category.AddProperty(sourceFilePathProp)
	.CustomWidget()
	.NameContent()
	[
		sourceFilePathProp->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MaxDesiredWidth(400)
	[
		SNew(SMounteaSourceFilePathWidget)
		.OnGetFilePath_Lambda([sourceFilePathProp]() -> FString
		{
			FString value;
			sourceFilePathProp->GetValue(value);
			return value;
		})
	];
}
