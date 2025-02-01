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
				"UnrealEd"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] 
			{
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[] 
			{
			}
		);
	}
}