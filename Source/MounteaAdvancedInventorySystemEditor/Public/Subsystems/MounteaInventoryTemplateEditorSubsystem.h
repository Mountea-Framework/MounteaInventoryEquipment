// All rights reserved Dominik Morse 2024

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "MounteaInventoryTemplateEditorSubsystem.generated.h"

class UMounteaInventoryItemTemplate;

/**
 * 
 */
UCLASS()
class MOUNTEAADVANCEDINVENTORYSYSTEMEDITOR_API UMounteaInventoryTemplateEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	UMounteaInventoryItemTemplate* GetOrCreateTempTemplate();
	void ClearTempTemplate();
	bool HasTempTemplate() const;

private:
	UPROPERTY()
	TObjectPtr<UMounteaInventoryItemTemplate> TempTemplate;
};
