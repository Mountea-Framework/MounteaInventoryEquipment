using UnrealBuildTool;
 
public class MounteaInventoryEquipmentEditor : ModuleRules
{
	public MounteaInventoryEquipmentEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange
		(
			new string[]
			{
				"Core", 
				"CoreUObject", 
				"Engine", 
				"UnrealEd",
				"Projects",
				
				"BlueprintGraph",
				"SlateCore",
				"KismetCompiler",

				"UnrealEd",
				
				"AssetTools",
			}
		);
		
		PrivateDependencyModuleNames.AddRange
		(
			new string[]
			{
				"MounteaInventoryEquipment", 

				"AssetTools",
				
				"Slate",
				"SlateCore",
				"GraphEditor",
				"PropertyEditor",
				"EditorStyle",
				"Kismet",
				"KismetWidgets",
				"ApplicationCore",
				"ToolMenus",
				"DeveloperSettings",
				"Projects", 
				
				"WebBrowser",
				"HTTP", 
				"Json", 
				"JsonUtilities",
				
				"BlueprintGraph",
				"InputCore", 
				
				"MainFrame",
				"ContentBrowser", 
				"DataTableEditor",
				"DesktopPlatform",
				
				"GameplayTags",
				"KismetCompiler", 
				
				"GameplayTagsEditor"
			}
		);
 
		PublicIncludePaths.AddRange
		(
			new string[]
			{
				//...
			}
		);
		
		PrivateIncludePaths.AddRange
		(
			new string[]
			{
				//...
			}
		);
	}
}