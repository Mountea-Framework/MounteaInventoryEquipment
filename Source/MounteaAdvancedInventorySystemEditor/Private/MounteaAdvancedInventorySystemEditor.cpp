#include "MounteaAdvancedInventorySystemEditor.h"

DEFINE_LOG_CATEGORY(MounteaAdvancedInventorySystemEditor);

#define LOCTEXT_NAMESPACE "FMounteaAdvancedInventorySystemEditor"

void FMounteaAdvancedInventorySystemEditor::StartupModule()
{
	UE_LOG(MounteaAdvancedInventorySystemEditor, Warning, TEXT("MounteaAdvancedInventorySystemEditor module has been loaded"));
}

void FMounteaAdvancedInventorySystemEditor::ShutdownModule()
{
	UE_LOG(MounteaAdvancedInventorySystemEditor, Warning, TEXT("MounteaAdvancedInventorySystemEditor module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMounteaAdvancedInventorySystemEditor, MounteaAdvancedInventorySystemEditor)