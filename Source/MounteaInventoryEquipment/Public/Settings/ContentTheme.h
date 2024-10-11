// All rights reserved Dominik Morse (Pavlicek) 2024

#pragma once

#include "CoreMinimal.h"

#include "Definitions/MounteaInventoryItemCategory.h"
#include "Engine/DataAsset.h"
#include "Helpers/MounteaThemeConfigHelpers.h"
#include "ContentTheme.generated.h"

#define LOCTEXT_NAMESPACE "ContentTheme"

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
	class UImageTheme* CategoryStyle = nullptr;

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
 * A theme configuration dedicated specifically for styling text elements within the application.
 * UTextTheme provides detailed customization options for both primary and secondary text styles, 
 * allowing developers and designers to ensure consistency and thematic coherence in text presentations.
 *
 * This configuration encapsulates:
 * - Primary Text: Settings related to the font, color, and other visual attributes for main or headline text.
 * - Secondary Text: Settings for text that might act as subtitles, descriptions, or any secondary information.
 * 
 * Given its Blueprintable nature, users can effortlessly spawn unique instances of this theme configuration directly from the Blueprint Editor.
 * This promotes rapid iterations and experiments with text themes without the necessity of C++ involvement.
 * 
 * Usage:
 * Integrate an instance of this theme configuration into the relevant classes, like UInventoryThemeConfig, to enforce the defined text styles.
 * 
 * @see UContentTheme
 * @see UInventoryThemeConfig
 */
UCLASS( DisplayName="Text Theme Config")
class MOUNTEAINVENTORYEQUIPMENT_API UTextTheme : public UContentTheme
{
	GENERATED_BODY()

public:
	
	/** Styling settings for primary text elements. */
	UPROPERTY(EditAnywhere, Category="Primary Text Theme")
	FTextSettings PrimaryTextTheme;

	/** Styling settings for secondary or less emphasized text elements. */
	UPROPERTY(EditAnywhere, Category="Secondary Text Theme")
	FTextSettings SecondaryTextTheme;
};

/**
 * UImageTheme
 *
 * A dedicated theme configuration focused on the visual styling of image elements within the application.
 * UImageTheme provides a centralized setting to dictate the appearance and presentation of images, 
 * ensuring consistent visual treatment and thematic alignment across various UI and content elements.
 *
 * The key configuration encapsulated within this class includes:
 * - Image Style: Governs the visual attributes like texture, tint, and material effects for the images. 
 *   This primary setting ensures that all images adhere to the established visual guidelines and thematic constraints.
 * 
 * With its Blueprintable attribute, users can create unique variations of this theme configuration directly from the Blueprint Editor,
 * facilitating quick iterations and visual tweaks without delving into C++.
 * 
 * Usage:
 * Integrate an instance of this theme configuration into the relevant classes to apply the defined image style consistently across the application.
 * 
 * @see UContentTheme
 * @see UInventoryThemeConfig
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
 * A specialized theme configuration centered on defining a color palette for the application. The UColoursTheme
 * provides a comprehensive set of colors, from primary to quaternary, ensuring that designers and developers 
 * have a consistent and harmonious color set to work with throughout the UI and content spaces.
 *
 * Each color within this configuration serves a distinct purpose:
 * - Primary Color: The mainstay of the UI. It represents primary actions, elements in focus, and main highlights.
 * - Secondary Color: A backdrop shade, typically used for background fills, secondary components, or elements in a passive state.
 * - Tertiary Color: A supplementary hue for elements like hover states, accents, or any intermediate UI component that requires differentiation.
 * - Quaternary Color: An auxiliary color that can be utilized for specific theming needs, such as borders, outlines, or unique accents.
 * 
 * Being Blueprintable, this configuration can be tweaked and iterated upon directly from the Blueprint Editor. This flexibility ensures 
 * that UI/UX designers can experiment and finalize the color theme without the need to modify C++ code.
 * 
 * Usage:
 * Integrate an instance of this theme configuration into the relevant classes to apply the defined color palette consistently 
 * across various UI and content sections of the application.
 * 
 * @see UContentTheme
 * @see UInventoryThemeConfig
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
 * A dedicated theme configuration tailored for button elements within the application. The UButtonTheme focuses on
 * providing customization settings to define the appearance, behavior, and overall style of button elements, ensuring 
 * a consistent and visually appealing user interaction across different areas of the application.
 *
 * The configuration encapsulates:
 * - Button Style: This captures the visual representation of the button, including its default state, hovered state, 
 *   pressed state, and more. It also can incorporate information like padding, font, and color settings, allowing 
 *   for a comprehensive customization of button appearances.
 * 
 * As with other theme configurations, UButtonTheme is Blueprintable, enabling designers to adjust and set button styles 
 * directly from the Blueprint Editor without the need to edit C++ code. This encourages rapid iterations and flexibility 
 * in UI design, allowing for easy adaptability and refinements.
 * 
 * Usage:
 * Integrate an instance of this theme configuration into the relevant classes or UI components where button styling 
 * needs to be applied. This ensures uniformity in button appearances and interactions across the application.
 * 
 * @see UContentTheme
 * @see UInventoryThemeConfig
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

/**
 * USlotTheme
 *
 * This theme configuration is specifically crafted for inventory slots, enabling customization of various layers 
 * within each slot. A slot in inventory or UI systems often has multiple visual elements layered together to create 
 * a rich and interactive appearance. The USlotTheme offers a structured way to theme these layers, ensuring visual 
 * consistency across all slots in the application.
 *
 * The configuration covers the following layers:
 * - Primary Slot Layer: This forms the base or background layer of the slot, often used for the main appearance.
 * - Secondary Slot Layer: This sits atop the primary layer, possibly representing additional design elements or subtle details.
 * - Tertiary Slot Layer: Another layer for decorative or functional purposes, such as hover effects or highlight states.
 * - Quaternary Slot Layer: The top-most layer which can be used for special accents, borders, or additional overlays.
 * 
 * Given its Blueprintable nature, designers can easily customize the look and feel of slots directly within the 
 * Blueprint Editor, facilitating quick iterations and real-time visual feedback. This level of customization ensures 
 * that slots in the application can be themed to match any desired aesthetic or design language.
 * 
 * Usage:
 * For any UI component or system that utilizes slots, apply an instance of this theme configuration to achieve the 
 * desired slot appearance. Combining multiple layers provides depth and richness to the slot design, enhancing 
 * user experience.
 * 
 * @see UContentTheme
 * @see UInventoryThemeConfig
 */
UCLASS( DisplayName="Slot Theme Config")
class MOUNTEAINVENTORYEQUIPMENT_API USlotTheme : public UContentTheme
{
	GENERATED_BODY()

public:
	
	
	UPROPERTY(EditAnywhere, Category="Slot Brush")
	FSlateBrush PrimarySlotLayer;

	UPROPERTY(EditAnywhere, Category="Slot Brush")
	FSlateBrush SecondarySlotLayer;

	UPROPERTY(EditAnywhere, Category="Slot Brush")
	FSlateBrush TertiarySlotLayer;

	UPROPERTY(EditAnywhere, Category="Slot Brush")
	FSlateBrush QuaternarySlotLayer;
};

#undef LOCTEXT_NAMESPACE