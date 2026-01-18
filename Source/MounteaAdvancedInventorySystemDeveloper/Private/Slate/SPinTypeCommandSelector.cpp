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

#if WITH_EDITOR

#include "Slate/SPinTypeCommandSelector.h"

#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "Settings/MounteaAdvancedInventorySettings.h"
#include "Settings/MounteaAdvancedInventoryUIConfig.h"

void SPinTypeCommandSelector::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
    SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
    
    TArray<FName> commands;
    GetWidgetCommandsFromSettings(commands);
    
    CommandOptions.Empty(commands.Num());
    for (const FName& cmd : commands)
    {
        CommandOptions.Add(MakeShared<FString>(cmd.ToString()));
    }
    
    if (CommandOptions.Num() > 0 && GraphPinObj && GraphPinObj->GetDefaultAsString().IsEmpty() && GraphPinObj->LinkedTo.Num() == 0)
        GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, *CommandOptions[0]);
}

void SPinTypeCommandSelector::GetWidgetCommandsFromSettings(TArray<FName>& OutCommands)
{
    OutCommands.Empty();
	
    const auto settings = GetDefault<UMounteaAdvancedInventorySettings>();
    if (!settings) return;
	
    const UMounteaAdvancedInventoryUIConfig* uiConfig = settings->InventoryUISettingsConfig.LoadSynchronous();
    if (!uiConfig)
        return;

    TArray<FString> widgetCommands = uiConfig->WidgetCommands.Array();
    widgetCommands.Sort();
	
    for (const FString& Command : widgetCommands)
    {
        OutCommands.Add(FName(*Command));
    }
}

TSharedRef<SWidget> SPinTypeCommandSelector::GetDefaultValueWidget()
{
    return SNew(SBox)
        .Visibility(this, &SPinTypeCommandSelector::GetDefaultValueVisibility)
        [
            SNew(SComboBox<TSharedPtr<FString>>)
            .ContentPadding(FMargin(2.0f, 2.0f))
            .OptionsSource(&CommandOptions)
            .OnGenerateWidget(this, &SPinTypeCommandSelector::GenerateCommandWidget)
            .OnSelectionChanged(this, &SPinTypeCommandSelector::OnCommandSelected)
            [
                SNew(STextBlock)
                .Text(this, &SPinTypeCommandSelector::GetSelectedCommandText)
                .Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
            ]
        ];
}

EVisibility SPinTypeCommandSelector::GetDefaultValueVisibility() const
{
    return (GraphPinObj && GraphPinObj->LinkedTo.Num() > 0) ? EVisibility::Collapsed : EVisibility::Visible;
}

void SPinTypeCommandSelector::OnCommandSelected(TSharedPtr<FString> Selection, ESelectInfo::Type SelectInfo)
{
    if (Selection.IsValid() && GraphPinObj)
    {
        const FString selectedCommand = *Selection;
        if (GraphPinObj->GetDefaultAsString() != selectedCommand)
        {
            const FScopedTransaction transaction(NSLOCTEXT("CommandPin", "ChangeCommand", "Change Widget Command"));
            GraphPinObj->Modify();
            GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, selectedCommand);
        }
    }
}

FText SPinTypeCommandSelector::GetSelectedCommandText() const
{
    if (GraphPinObj)
        return FText::FromString(GraphPinObj->GetDefaultAsString());
    return FText::GetEmpty();
}

TSharedRef<SWidget> SPinTypeCommandSelector::GenerateCommandWidget(TSharedPtr<FString> Item)
{
    return SNew(STextBlock)
        .Text(FText::FromString(*Item))
        .Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
}

#endif