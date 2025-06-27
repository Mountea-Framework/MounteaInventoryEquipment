using UnrealBuildTool;

public class MounteaAdvancedInventorySystemEditor : ModuleRules
{
	public MounteaAdvancedInventorySystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] 
			{
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] 
			{
				"MounteaAdvancedInventorySystemEditor/Private"
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[] 
			{
				"Core", 
				"CoreUObject", 
				"Engine", 
				"UnrealEd",
				"AssetTools",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] 
			{
				"MounteaAdvancedInventorySystem",

				"UnrealEd",
				"DeveloperSettings",
				"Projects",

				"Slate",
				"SlateCore",
				"EditorStyle",

				"BlueprintGraph",
				"ToolMenus",
				"AssetTools",
				"GraphEditor",
				"PropertyEditor",
				"Kismet",
				"KismetWidgets",
				"ApplicationCore",
				"MainFrame",

				"InputCore",
				"GameplayTags",
				"GameplayTagsEditor",
				"UMGEditor",
				
				"Blutility",
				
				"DesktopPlatform", 
				
				"EditorSubsystem", 
				"EditorScriptingUtilities",
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[] 
			{
			}
		);
	}
}