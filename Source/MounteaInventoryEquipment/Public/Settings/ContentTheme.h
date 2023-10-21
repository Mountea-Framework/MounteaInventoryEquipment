// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Helpers/MounteaThemeConfigHelpers.h"
#include "ContentTheme.generated.h"

USTRUCT(BlueprintType)
struct FContentThemeConfigData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Inventory", meta=(NoResetToDefault, AllowAbstract=false, BlueprintBaseOnly=true))
	class UContentThemeConfig* ContentThemeConfig = nullptr;
};

UCLASS( Abstract, Blueprintable, BlueprintType, EditInlineNew, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Content Theme Config")
class MOUNTEAINVENTORYEQUIPMENT_API UContentThemeConfig : public UObject
{
	GENERATED_BODY()
	
};

UCLASS()
class MOUNTEAINVENTORYEQUIPMENT_API UCategoryThemeConfig : public UContentThemeConfig
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="Settings")
	FVector2D IconBrushCoordinates;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------------------------//

USTRUCT(BlueprintType)
struct FContentThemeData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Inventory", meta=(NoResetToDefault, AllowAbstract=false, BlueprintBaseOnly=true))
	class UContentTheme* ContentTheme = nullptr;
};

/**
 * UContentTheme
 *
 * This class acts as a foundational DataAsset, designed to encapsulate various theme settings
 * that pertain to content styling and presentation. Given its role as a base class, UContentTheme establishes 
 * a standard structure and provides a common set of properties that all themed configurations should possess.
 *
 * The primary motivation behind UContentTheme is to offer a unified and consistent approach to theming across 
 * different areas of our application, ensuring a harmonized visual and functional experience for the end-users.
 *
 * Key Features:
 * 1. **Extensibility**: Derived classes can augment and specialize the base properties, tailoring theme configurations 
 *    to specific needs. This allows for rich, diverse theming capabilities without deviating from the established core structure.
 * 2. **Blueprint-friendly**: Not just limited to C++, UContentThemeConfig is designed to be extended via Blueprints. This 
 *    empowers designers and other non-programming team members to create and experiment with new theme variations with ease.
 * 3. **Centralized Theme Management**: By using UInventoryThemeConfig, container for UContentThemeConfigs, we ensure that
 *    theme configurations for inventory and related UI elements are maintained centrally, promoting consistency and ease of updates.
 *
 * Usage:
 * - Create a new C++ or Blueprint class derived from UContentThemeConfig.
 * - Expand upon the base properties or create new ones to cater to the specific theme requirements.
 * - Use the derived class within the application, ensuring a consistent and theme-adherent presentation.
 *
 * Note: As this class is intended to store static data, any dynamic theming or real-time adjustments should be handled 
 * externally, with the results then applied in accordance with the configurations defined in the relevant UContentTheme derivative.
 */
UCLASS( Abstract, Blueprintable, EditInlineNew, BlueprintType, ClassGroup=("Mountea"), AutoExpandCategories=("Mountea, Inventory"), DisplayName="Content Theme Config")
class MOUNTEAINVENTORYEQUIPMENT_API UContentTheme : public UDataAsset
{
	GENERATED_BODY()
	
};

UCLASS( DisplayName="Category Theme Config")
class MOUNTEAINVENTORYEQUIPMENT_API UCategoryTheme : public UContentTheme
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="Icon Brush")
	FTextSettings CategoryTextSettings;

	UPROPERTY(EditAnywhere, Category="Text Brush")
	FSlateBrush CategoryStyle;
};