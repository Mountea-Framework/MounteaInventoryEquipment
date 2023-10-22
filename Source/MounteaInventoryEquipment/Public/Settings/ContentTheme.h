// All rights reserved Dominik Pavlicek 2023.

#pragma once

#include "CoreMinimal.h"
#include "Definitions/MounteaInventoryItemCategory.h"
#include "Engine/DataAsset.h"
#include "Helpers/MounteaThemeConfigHelpers.h"
#include "ContentTheme.generated.h"

USTRUCT(BlueprintType)
struct FContentThemeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Inventory", meta=(NoResetToDefault, AllowAbstract=false, BlueprintBaseOnly=true))
	class UContentTheme* ContentTheme = nullptr;

public:

	bool operator==(const FContentThemeData& Other) const
	{
		return ContentTheme == Other.ContentTheme;
	}
	
	friend uint32 GetTypeHash(const FContentThemeData& Data)
	{				
		return Data.ContentTheme ? GetTypeHash(Data.ContentTheme) : 0;
	}
};

USTRUCT(BlueprintType)
struct FCategoryThemeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Inventory", meta=(NoResetToDefault, AllowAbstract=false, BlueprintBaseOnly=true))
	class UCategoryTheme* CategoryTheme = nullptr;

public:

	bool operator==(const FCategoryThemeData& Other) const
	{
		return CategoryTheme == Other.CategoryTheme;
	}

	friend uint32 GetTypeHash(const FCategoryThemeData& Data)
	{				
		return Data.CategoryTheme ? GetTypeHash(Data.CategoryTheme) : 0;
	}
};

USTRUCT(BlueprintType)
struct FTextThemeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Inventory", meta=(NoResetToDefault, AllowAbstract=false, BlueprintBaseOnly=true))
	class UTextTheme* TextTheme = nullptr;

public:

	bool operator==(const FTextThemeData& Other) const
	{
		return TextTheme == Other.TextTheme;
	}

	friend uint32 GetTypeHash(const FTextThemeData& Data)
	{				
		return Data.TextTheme ? GetTypeHash(Data.TextTheme) : 0;
	}
};

USTRUCT(BlueprintType)
struct FColoursThemeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Inventory", meta=(NoResetToDefault, AllowAbstract=false, BlueprintBaseOnly=true))
	class UColoursTheme* ColoursTheme = nullptr;

public:

	bool operator==(const FColoursThemeData& Other) const
	{
		return ColoursTheme == Other.ColoursTheme;
	}

	friend uint32 GetTypeHash(const FColoursThemeData& Data)
	{				
		return Data.ColoursTheme ? GetTypeHash(Data.ColoursTheme) : 0;
	}
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

/**
 * UCategoryThemeConfig
 * 
 * A specialized theme configuration tailored for category-specific content presentation within the inventory system.
 * This class extends the base UContentThemeConfig to provide further customization and theming options specific to inventory categories.
 * It allows designers and developers to define the visual and interactive characteristics of category elements, ensuring consistent and theme-driven user experiences.
 * 
 * This configuration includes settings for:
 * - Category Text: Defines the font, color, and other text-related settings for displaying category names or descriptions.
 * - Category Style: Determines the visual appearance, including images and material effects, for category backgrounds, borders, or icons.
 * 
 * By being Blueprintable, users can create unique instances of this theme configuration through Blueprints, allowing easy and flexible customization without diving into C++.
 * 
 * Usage:
 * Attach an instance of this configuration to the UInventoryThemeConfig or any relevant class to apply theme settings.
 * 
 * @see UContentThemeConfig
 * @see UInventoryThemeConfig
 */
UCLASS( DisplayName="Category Theme Config")
class MOUNTEAINVENTORYEQUIPMENT_API UCategoryTheme : public UContentTheme
{
	GENERATED_BODY()

public:

	/** The specific inventory item category this theme config pertains to. */
	UPROPERTY(EditAnywhere, Category="Theme Config")
	UMounteaInventoryItemCategory* Category = nullptr;

	/** Defines the appearance settings for the category, such as backgrounds, borders, or icons. */
	UPROPERTY(EditAnywhere, Category="Theme Config")
	class UImageTheme* CategoryStyle;

	/** Adjusts the position of the category's icon. Used only for specific UImageStyle with Texture Atlas. */
	UPROPERTY(EditAnywhere, Category="Theme Config")
	FVector2D CategoryIconOffset;

	/** Allows for overriding the default text theme for this specific category. */
	UPROPERTY(EditAnywhere, Category="Theme Config|Override")
	FTextThemeData TextThemeOverride;
};

/**
 * UTextTheme
 *
 * Dedicated theme configuration for text elements. This class offers customization settings
 * to define the appearance and style of primary and secondary text elements.
 */
UCLASS( DisplayName="Text Theme Config")
class MOUNTEAINVENTORYEQUIPMENT_API UTextTheme : public UContentTheme
{
	GENERATED_BODY()

public:

	/** Styling settings for primary text elements. */
	UPROPERTY(EditAnywhere, Category="Text Theme")
	FTextSettings PrimaryTextTheme;

	/** Styling settings for secondary or less emphasized text elements. */
	UPROPERTY(EditAnywhere, Category="Text Theme")
	FTextSettings SecondaryTextTheme;
};

/**
 * UImageTheme
 *
 * This theme configuration is specialized for Slate Brush styling. It encapsulates settings that
 * define the appearance of various image elements within the system.
 */
UCLASS( DisplayName="Image Theme Config")
class MOUNTEAINVENTORYEQUIPMENT_API UImageTheme : public UContentTheme
{
	GENERATED_BODY()

public:
	
	/** The primary appearance settings for images. */
	UPROPERTY(EditAnywhere, Category="Text Brush")
	FSlateBrush ImageStyle;
};

/**
 * UColoursTheme
 *
 * Tailored to provide color theming across the application. It offers a set of color configurations
 * to establish visual harmony and branding consistency.
 */
UCLASS( DisplayName="Colours Theme Config")
class MOUNTEAINVENTORYEQUIPMENT_API UColoursTheme : public UContentTheme
{
	GENERATED_BODY()

public:
	
	/** The primary color typically used for primary UI elements and highlights. */
	UPROPERTY(EditAnywhere, Category="Colours", meta=(DisplayPriority=-100))
	FLinearColor PrimaryColor = FLinearColor(1,0.863157,0.502886);

	/** A secondary color used for background fills, secondary UI elements, or passive elements. */
	UPROPERTY(EditAnywhere, Category="Colours", meta=(DisplayPriority=-50))
	FLinearColor SecondaryColor = FLinearColor(0.230740,0.187821,0.122139);

	/** Used for tertiary purposes, such as hover states, accents, or other intermediate UI elements. */
	UPROPERTY(EditAnywhere, Category="Colours", meta=(DisplayPriority=-10))
	FLinearColor TertiaryColor = FLinearColor(1,1,1);

	/** A fourth-tier color that can be used for additional UI theming, like borders, outlines, or specific accents. */
	UPROPERTY(EditAnywhere, Category="Colours", meta=(DisplayPriority=-1))
	FLinearColor QuaternaryColor = FLinearColor(0.693872,0.351533,0.008568);
};

/**
 * UButtonTheme
 *
 * This theme configuration is specialized for Button Brush styling. It encapsulates settings that
 * define the appearance of Button types.
 */
UCLASS( DisplayName="Button Theme Config")
class MOUNTEAINVENTORYEQUIPMENT_API UButtonTheme : public UContentTheme
{
	GENERATED_BODY()

public:
	
	/** The primary appearance settings for images. */
	UPROPERTY(EditAnywhere, Category="Button Brush")
	FButtonStyle ButtonStyle;
};
