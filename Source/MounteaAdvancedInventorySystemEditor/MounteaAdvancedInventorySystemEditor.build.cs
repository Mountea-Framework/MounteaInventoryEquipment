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
				
				"DeveloperSettings",
				"Projects",
				
				"Slate",
				"SlateCore",
				
				"BlueprintGraph",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] 
			{
				"MounteaAdvancedInventorySystem"
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[] 
			{
			}
		);
	}
}