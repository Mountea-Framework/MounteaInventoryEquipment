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

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/MounteaAdvancedInventoryBaseWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Widgets/MounteaInventoryWidgetThemeExtension.h"
#include "Components/VerticalBox.h"
#include "Settings/MounteaAdvancedInventoryGlobalUIConfig.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventoryThemeConfig.h"
#include "Settings/MounteaAdvancedInventoryUIConfig.h"
#include "Engine/Texture2D.h"
#include "Statics/MounteaInventoryBaseUIStatics.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaWidgetThemeLifetimeTest,
	"Mountea.Inventory.UI.Theme.PrefixAndLifetime",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMounteaWidgetThemeLifetimeTest::RunTest(const FString& Parameters)
{
	auto* settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	const auto originalConfig = settings->GlobalUIConfig;
	ON_SCOPE_EXIT
	{
		settings->GlobalUIConfig = originalConfig;
	};

	auto* config = NewObject<UMounteaAdvancedInventoryGlobalUIConfig>();
	settings->GlobalUIConfig = config;
	FMounteaWidgetPrefixGroup group;
	group.WidgetClass = UTextBlock::StaticClass();
	FMounteaWidgetPrefixRule rule;
	rule.Prefixes.Add(TEXT("TextPrimary_"));
	rule.Style.TypeRole = TEXT("Body");
	group.Rules.Add(rule);
	FMounteaWidgetPrefixRule fallbackRule;
	fallbackRule.Prefixes.Add(TEXT("TextDefault_"));
	fallbackRule.Style.TypeRole = TEXT("Fallback");
	group.Rules.Add(fallbackRule);
	config->WidgetPrefixGroups.Add(group);
	FMounteaWidgetPrefixGroup fadeGroup;
	fadeGroup.WidgetClass = UWidget::StaticClass();
	FMounteaWidgetPrefixRule fadeRule;
	fadeRule.Prefixes.Add(TEXT("FadeGroup_"));
	fadeRule.Style.StyleType = EMounteaWidgetThemeStyleType::GroupOpacity;
	fadeRule.Style.GroupOpacityRole = TEXT("Subtree");
	fadeGroup.Rules.Add(fadeRule);
	config->WidgetPrefixGroups.Add(fadeGroup);
	FMounteaWidgetPrefixGroup borderGroup;
	borderGroup.WidgetClass = UBorder::StaticClass();
	FMounteaWidgetPrefixRule borderRule;
	borderRule.Prefixes.Add(TEXT("BorderPrimary_"));
	borderRule.Style.StyleType = EMounteaWidgetThemeStyleType::Brush;
	borderRule.Style.BrushRole = TEXT("Panel");
	borderRule.Style.TintPaint = TEXT("Primary");
	borderGroup.Rules.Add(borderRule);
	config->WidgetPrefixGroups.Add(borderGroup);
	FMounteaWidgetPrefixGroup progressGroup;
	progressGroup.WidgetClass = UProgressBar::StaticClass();
	FMounteaWidgetPrefixRule progressRule;
	progressRule.Prefixes.Add(TEXT("ProgressPrimary_"));
	progressRule.Style.StyleType = EMounteaWidgetThemeStyleType::Control;
	progressRule.Style.ControlRole = TEXT("Progress");
	progressRule.Style.TintPaint = TEXT("Primary");
	progressGroup.Rules.Add(progressRule);
	config->WidgetPrefixGroups.Add(progressGroup);

	auto* theme = NewObject<UMounteaAdvancedInventoryThemeConfig>();
	theme->Palette.Add(TEXT("Ink"), FLinearColor(0.3f, 0.4f, 0.5f, 0.1f));
	FMounteaThemePaint paint;
	paint.Ink = TEXT("Ink");
	paint.Opacity = 0.62f;
	theme->Paints.Add(TEXT("Primary"), paint);
	FMounteaThemeTextStyle type;
	type.Foreground = TEXT("Primary");
	theme->Typography.Add(TEXT("Body"), type);
	theme->Typography.Add(TEXT("Fallback"), FMounteaThemeTextStyle());
	theme->Palette.Add(theme->DefaultInk, FLinearColor::Blue);
	theme->GroupOpacity.Add(TEXT("Subtree"), 0.45f);
	theme->Brushes.Add(TEXT("Panel"), FMounteaThemeBrushStyle());
	theme->Controls.Add(TEXT("Progress"), FMounteaThemeControlStyle());

	auto* owner = NewObject<UMounteaAdvancedInventoryBaseWidget>();
	TestFalse(TEXT("A missing tree leaves initialization pending"), UMounteaInventoryBaseUIStatics::ApplyWidgetThemeByPrefix(owner, theme));
	TestEqual(TEXT("A pending instance retains its resolved screen context"), UMounteaInventoryBaseUIStatics::GetWidgetTheme(owner), theme);
	owner->WidgetTree = NewObject<UWidgetTree>(owner);
	auto* panel = owner->WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("FadeGroup_Root"));
	owner->WidgetTree->RootWidget = panel;
	auto* matched = owner->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("textprimary_Label_More"));
	auto* unmatched = owner->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TextPrimaryExtra_Label"));
	auto* emptySuffix = owner->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TextPrimary_"));
	auto* fallback = owner->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TextDefault_Label"));
	panel->AddChild(fallback);
	panel->AddChild(matched);
	panel->AddChild(unmatched);
	panel->AddChild(emptySuffix);
	auto* border = owner->WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BorderPrimary_Panel"));
	auto* progress = owner->WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("ProgressPrimary_Crafting"));
	border->SetBrushColor(FLinearColor::Red);
	progress->SetFillColorAndOpacity(FLinearColor::Red);
	panel->AddChild(border);
	panel->AddChild(progress);
	const FLinearColor authored = FLinearColor::Yellow;
	unmatched->SetColorAndOpacity(authored);
	emptySuffix->SetColorAndOpacity(authored);

	auto* child = NewObject<UMounteaAdvancedInventoryBaseWidget>(owner->WidgetTree);
	child->WidgetTree = NewObject<UWidgetTree>(child);
	auto* childText = child->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TextPrimary_Child"));
	child->WidgetTree->RootWidget = childText;
	panel->AddChild(child);

	owner->GetExtension<UMounteaInventoryWidgetThemeExtension>()->Construct();
	TestTrue(TEXT("A late tree initializes through its retained extension"), owner->GetExtension<UMounteaInventoryWidgetThemeExtension>()->Initialized);
	const FLinearColor expected(0.3f, 0.4f, 0.5f, 0.62f);
	TestEqual(TEXT("Whole prefix is case insensitive and paint alpha replaces palette alpha"), matched->GetColorAndOpacity().GetSpecifiedColor(), expected);
	TestEqual(TEXT("Typography without a foreground uses the theme default ink"), fallback->GetColorAndOpacity().GetSpecifiedColor(), FLinearColor::Blue);
	TestEqual(TEXT("A partial prefix stays authored"), unmatched->GetColorAndOpacity().GetSpecifiedColor(), authored);
	TestEqual(TEXT("An empty suffix stays authored"), emptySuffix->GetColorAndOpacity().GetSpecifiedColor(), authored);
	TestEqual(TEXT("Nested widget retains the screen theme"), UMounteaInventoryBaseUIStatics::GetWidgetTheme(child), theme);
	TestEqual(TEXT("Nested widget styles its own root"), childText->GetColorAndOpacity().GetSpecifiedColor(), expected);
	TestEqual(TEXT("An explicit group role sets subtree opacity"), panel->GetRenderOpacity(), 0.45f);
	TestEqual(TEXT("Border tint paint updates its appearance multiplier"), border->GetBrushColor(), expected);
	TestEqual(TEXT("Progress tint paint updates its fill multiplier"), progress->GetFillColorAndOpacity(), expected);

	auto* dynamicChild = NewObject<UMounteaAdvancedInventoryBaseWidget>();
	dynamicChild->WidgetTree = NewObject<UWidgetTree>(dynamicChild);
	auto* dynamicText = dynamicChild->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TextPrimary_Dynamic"));
	dynamicChild->WidgetTree->RootWidget = dynamicText;
	panel->AddChild(dynamicChild);
	TestEqual(TEXT("An attached child can resolve its screen through its panel parent"), UMounteaInventoryBaseUIStatics::GetWidgetTheme(dynamicChild), theme);
	TestTrue(TEXT("A later child initializes from retained owner context"), UMounteaInventoryBaseUIStatics::ApplyWidgetThemeByPrefix(dynamicChild, UMounteaInventoryBaseUIStatics::GetWidgetTheme(owner)));
	TestEqual(TEXT("Dynamic child receives the screen appearance"), dynamicText->GetColorAndOpacity().GetSpecifiedColor(), expected);

	matched->SetColorAndOpacity(FLinearColor::Red);
	TestTrue(TEXT("Repeated initialization succeeds without a reset"), UMounteaInventoryBaseUIStatics::ApplyWidgetThemeByPrefix(owner, theme));
	TestEqual(TEXT("Reattachment preserves current state"), matched->GetColorAndOpacity().GetSpecifiedColor(), FLinearColor::Red);
	auto* replacement = DuplicateObject<UMounteaAdvancedInventoryThemeConfig>(theme, GetTransientPackage());
	replacement->Palette[TEXT("Ink")] = FLinearColor::Green;
	replacement->GroupOpacity[TEXT("Subtree")] = 0.7f;
	TestTrue(TEXT("An explicit different theme reapplies a reused instance"), UMounteaInventoryBaseUIStatics::ApplyWidgetThemeByPrefix(owner, replacement));
	const FLinearColor replacementColour(0.f, 1.f, 0.f, 0.62f);
	TestEqual(TEXT("Reused root receives the replacement theme"), matched->GetColorAndOpacity().GetSpecifiedColor(), replacementColour);
	TestEqual(TEXT("Replacement theme propagates to an existing nested widget"), childText->GetColorAndOpacity().GetSpecifiedColor(), replacementColour);
	TestEqual(TEXT("Replacement theme propagates to a late child"), dynamicText->GetColorAndOpacity().GetSpecifiedColor(), replacementColour);
	TestEqual(TEXT("Replacement group opacity applies"), panel->GetRenderOpacity(), 0.7f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMounteaThemeCategoryIconsTest,
	"Mountea.Inventory.UI.Theme.CategoryIcons",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMounteaThemeCategoryIconsTest::RunTest(const FString& Parameters)
{
	auto* settings = GetMutableDefault<UMounteaAdvancedInventorySettings>();
	const auto originalConfig = settings->GlobalUIConfig;
	ON_SCOPE_EXIT
	{
		settings->GlobalUIConfig = originalConfig;
	};

	auto* config = NewObject<UMounteaAdvancedInventoryGlobalUIConfig>();
	auto* theme = NewObject<UMounteaAdvancedInventoryThemeConfig>();
	auto* screen = NewObject<UMounteaAdvancedInventoryUIConfig>();
	settings->GlobalUIConfig = config;
	config->BaseTheme = theme;

	const TSoftObjectPtr<UTexture2D> parentIcon(FSoftObjectPath(TEXT("/Game/ThemeTest/Parent.Parent")));
	const TSoftObjectPtr<UTexture2D> childIcon(FSoftObjectPath(TEXT("/Game/ThemeTest/Child.Child")));
	const TSoftObjectPtr<UTexture2D> overrideIcon(FSoftObjectPath(TEXT("/Game/ThemeTest/Override.Override")));
	auto& icons = theme->CategoryIcons.Add(TEXT("weapon-key"));
	icons.Icon = parentIcon;
	icons.SubCategories.Add(TEXT("sword-key"), childIcon);
	icons.SubCategories.Add(TEXT("unset-key"), TSoftObjectPtr<UTexture2D>());

	TestEqual(TEXT("Parent uses the active global theme"), UMounteaInventoryBaseUIStatics::GetCategoryIcon(TEXT("weapon-key")), parentIcon);
	TestEqual(TEXT("Explicit child overrides its parent"), UMounteaInventoryBaseUIStatics::GetCategoryIcon(TEXT("weapon-key"), TEXT("sword-key")), childIcon);
	TestEqual(TEXT("Unset child inherits its parent"), UMounteaInventoryBaseUIStatics::GetCategoryIcon(TEXT("weapon-key"), TEXT("unset-key")), parentIcon);
	TestEqual(TEXT("Missing child inherits its parent"), UMounteaInventoryBaseUIStatics::GetCategoryIcon(TEXT("weapon-key"), TEXT("missing-key")), parentIcon);
	TestTrue(TEXT("Missing parent returns an empty reference"), UMounteaInventoryBaseUIStatics::GetCategoryIcon(TEXT("missing-key")).IsNull());
	TestEqual(TEXT("Empty screen override inherits global theme"), UMounteaInventoryBaseUIStatics::GetCategoryIcon(TEXT("weapon-key"), FString(), screen), parentIcon);

	auto* replacement = NewObject<UMounteaAdvancedInventoryThemeConfig>();
	replacement->CategoryIcons.Add(TEXT("weapon-key")).Icon = overrideIcon;
	screen->ThemeOverride = replacement;
	TestEqual(TEXT("Screen theme takes precedence and supplies its own fallback"), UMounteaInventoryBaseUIStatics::GetCategoryIcon(TEXT("weapon-key"), TEXT("sword-key"), screen), overrideIcon);

	icons.Icon.Reset();
	TestTrue(TEXT("Unset parent remains empty"), UMounteaInventoryBaseUIStatics::GetCategoryIcon(TEXT("weapon-key"), TEXT("unset-key")).IsNull());
	TestEqual(TEXT("Child is usable with no parent texture"), UMounteaInventoryBaseUIStatics::GetCategoryIcon(TEXT("weapon-key"), TEXT("sword-key")), childIcon);
	config->BaseTheme.Reset();
	TestTrue(TEXT("Missing theme returns an empty reference"), UMounteaInventoryBaseUIStatics::GetCategoryIcon(TEXT("weapon-key")).IsNull());

	return true;
}

#endif
