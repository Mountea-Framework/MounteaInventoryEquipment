// Copyright (C) 2026 Dominik (Pavlicek) Morse. All rights reserved.
//
// Developed for the Mountea Framework as a free tool. This solution is provided
// for use and sharing without charge. Redistribution is allowed under the following conditions:
//
// - You may use this solution in commercial products, provided the product is not
//   this solution itself (or unless significant modifications have been made to the solution).
// - You may not resell or redistribute the original, unmodified solution.
//
// For more information, visit: https://mountea.tools

#include "Slate/SMounteaJsonDefinitionDependencyList.h"

#include "Editor.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Styling/MounteaAdvancedInventoryEditorStyle.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "MounteaJsonDefinitionDependencyList"

namespace
{
	const FLinearColor BlueprintLabelColor(0.35f, 0.6f, 1.f);
	const FLinearColor CppLabelColor(0.6f, 0.6f, 0.6f);
	const FLinearColor FolderTextColor(0.5f, 0.5f, 0.5f);

	void OpenBlueprint(const FString& ObjectPath)
	{
		UObject* asset = FSoftObjectPath(ObjectPath).TryLoad();

		if (asset && GEditor)
			GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(asset);
	}

	TSharedRef<SWidget> MakeSectionLabel(const FText& Text, const FLinearColor& Color)
	{
		return SNew(STextBlock)
		.Text(Text)
		.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
		.ColorAndOpacity(Color);
	}

	TSharedRef<SWidget> MakeUsageCount(const int32 Count)
	{
		return SNew(STextBlock)
		.Text(FText::Format(LOCTEXT("UsageCount", "x{0}"), FText::AsNumber(Count)));
	}

	TSharedRef<SWidget> MakeBlueprintRow(const FMounteaJsonDefinitionUsage& Usage, const TWeakPtr<SWindow>& OwnerWindow)
	{
		const FString objectPath = Usage.Path;
		TSharedPtr<SBorder> borderWidget;

		const TSharedRef<SButton> button = SNew(SButton)
		.ButtonStyle(FAppStyle::Get(), "NoBorder")
		.ContentPadding(4)
		.OnClicked_Lambda([objectPath, OwnerWindow]()
		{
			OpenBlueprint(objectPath);

			if (const TSharedPtr<SWindow> pinned = OwnerWindow.Pin())
				pinned->RequestDestroyWindow();

			return FReply::Handled();
		})
		[
			SAssignNew(borderWidget, SBorder)
			.Padding(FMargin(10, 6))
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.FillWidth(1.f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(FPaths::GetBaseFilename(FPackageName::ObjectPathToPackageName(objectPath))))
					.ToolTipText(FText::FromString(objectPath))
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(8.f, 0.f, 0.f, 0.f)
				[
					MakeUsageCount(Usage.Count)
				]
			]
		];

		borderWidget->SetBorderImage(TAttribute<const FSlateBrush*>::CreateLambda([WeakButton = TWeakPtr<SButton>(button)]() -> const FSlateBrush*
		{
			const bool bHovered = WeakButton.IsValid() && WeakButton.Pin()->IsHovered();
			return FMounteaAdvancedInventoryEditorStyle::Get().GetBrush(bHovered ? "MAISStyleSet.TemplateItem.Hovered" : "MAISStyleSet.TemplateItem.Normal");
		}));

		return button;
	}

	TSharedRef<SWidget> MakeCppRow(const FMounteaJsonDefinitionUsage& Usage)
	{
		return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("NoBorder"))
		.Padding(FMargin(10, 6))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(FPaths::GetCleanFilename(Usage.Path)))
				.ToolTipText(FText::FromString(Usage.Path))
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(8.f, 0.f, 0.f, 0.f)
			[
				MakeUsageCount(Usage.Count)
			]
		];
	}
}

void SMounteaJsonDefinitionDependencyList::Construct(const FArguments& InArgs)
{
	const TArray<FMounteaJsonDefinitionUsage> usages = FMounteaJsonDefinitionDependencyScanner::GetUsages(InArgs._DefinitionKey);
	const TArray<FMounteaJsonDependencyGroup> groups = GroupBlueprintUsages(usages);

	int32 blueprintUses = 0;
	int32 blueprintLocations = 0;
	int32 cppLocations = 0;
	TArray<FMounteaJsonDefinitionUsage> cppUsages;
	for (const FMounteaJsonDefinitionUsage& usage : usages)
	{
		if (usage.Source == TEXT("Blueprint"))
		{
			blueprintUses += usage.Count;
			blueprintLocations++;
		}
		else
		{
			cppLocations++;
			cppUsages.Add(usage);
		}
	}

	const TSharedRef<SVerticalBox> list = SNew(SVerticalBox);

	list->AddSlot()
	.AutoHeight()
	.Padding(0.f, 0.f, 0.f, 6.f)
	[
		SNew(STextBlock)
		.Text(FText::FromString(InArgs._DefinitionKey))
		.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
	];

	if (!InArgs._Description.IsEmpty())
	{
		list->AddSlot()
		.AutoHeight()
		.Padding(0.f, 0.f, 0.f, 6.f)
		[
			SNew(STextBlock)
			.Text(InArgs._Description)
			.AutoWrapText(true)
		];
	}

	list->AddSlot()
	.AutoHeight()
	.Padding(0.f, 0.f, 0.f, 10.f)
	[
		SNew(STextBlock)
		.Text(FText::Format(
			LOCTEXT("Summary", "{0} use(s) in {1} Blueprint(s), {2} C++ file(s). Last scan: {3}"),
			FText::AsNumber(blueprintUses),
			FText::AsNumber(blueprintLocations),
			FText::AsNumber(cppLocations),
			FText::FromString(FMounteaJsonDefinitionDependencyScanner::GetLastScanTime())))
	];

	if (usages.IsEmpty())
	{
		list->AddSlot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("NoUsages", "No usages recorded. Run Recalculate Dependencies on the Global Config."))
			.AutoWrapText(true)
		];
	}

	if (!groups.IsEmpty())
	{
		list->AddSlot()
		.AutoHeight()
		.Padding(0.f, 4.f)
		[
			MakeSectionLabel(LOCTEXT("BlueprintsHeader", "Blueprints"), BlueprintLabelColor)
		];

		for (const FMounteaJsonDependencyGroup& group : groups)
		{
			list->AddSlot()
			.AutoHeight()
			.Padding(0.f, 6.f, 0.f, 2.f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(group.FolderPath))
				.ColorAndOpacity(FolderTextColor)
			];

			for (const FMounteaJsonDefinitionUsage& usage : group.Usages)
			{
				list->AddSlot()
				.AutoHeight()
				.Padding(0.f, 1.f)
				[
					MakeBlueprintRow(usage, InArgs._OwnerWindow)
				];
			}
		}
	}

	if (!cppUsages.IsEmpty())
	{
		list->AddSlot()
		.AutoHeight()
		.Padding(0.f, 12.f, 0.f, 4.f)
		[
			MakeSectionLabel(LOCTEXT("CppHeader", "C++"), CppLabelColor)
		];

		for (const FMounteaJsonDefinitionUsage& usage : cppUsages)
		{
			list->AddSlot()
			.AutoHeight()
			.Padding(0.f, 1.f)
			[
				MakeCppRow(usage)
			];
		}
	}

	ChildSlot
	[
		SNew(SScrollBox)

		+ SScrollBox::Slot()
		.Padding(12.f)
		[
			list
		]
	];
}

void SMounteaJsonDefinitionDependencyList::ShowModal(const FString& DefinitionKey, const FText& Description)
{
	const TSharedRef<SWindow> window = SNew(SWindow)
	.Title(FText::Format(LOCTEXT("WindowTitle", "Dependencies: {0}"), FText::FromString(DefinitionKey)))
	.ClientSize(FVector2D(560.f, 480.f))
	.SupportsMinimize(false)
	.SupportsMaximize(false);

	window->SetContent(
		SNew(SMounteaJsonDefinitionDependencyList)
		.DefinitionKey(DefinitionKey)
		.Description(Description)
		.OwnerWindow(window)
	);

	FSlateApplication::Get().AddModalWindow(window, FSlateApplication::Get().GetActiveTopLevelRegularWindow());
}

TArray<FMounteaJsonDependencyGroup> SMounteaJsonDefinitionDependencyList::GroupBlueprintUsages(const TArray<FMounteaJsonDefinitionUsage>& Usages)
{
	TArray<FMounteaJsonDependencyGroup> groups;
	for (const FMounteaJsonDefinitionUsage& usage : Usages)
	{
		if (usage.Source != TEXT("Blueprint"))
			continue;

		const FString folder = FPaths::GetPath(FPackageName::ObjectPathToPackageName(usage.Path));
		FMounteaJsonDependencyGroup* group = groups.FindByPredicate([&folder](const FMounteaJsonDependencyGroup& candidate)
		{
			return candidate.FolderPath == folder;
		});

		if (!group)
		{
			group = &groups.AddDefaulted_GetRef();
			group->FolderPath = folder;
		}

		group->Usages.Add(usage);
	}

	for (FMounteaJsonDependencyGroup& group : groups)
	{
		group.Usages.Sort([](const FMounteaJsonDefinitionUsage& A, const FMounteaJsonDefinitionUsage& B)
		{
			return A.Path < B.Path;
		});
	}

	groups.Sort([](const FMounteaJsonDependencyGroup& A, const FMounteaJsonDependencyGroup& B)
	{
		return A.FolderPath < B.FolderPath;
	});

	return groups;
}

#undef LOCTEXT_NAMESPACE
