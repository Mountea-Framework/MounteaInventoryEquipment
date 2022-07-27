using UnrealBuildTool;
 
public class ActorInventoryPluginEditor : ModuleRules
{
	public ActorInventoryPluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UnrealEd"});
 
		// PublicIncludePaths.AddRange(new string[] {"ActorInventoryPluginEditor/Public"});
		// PrivateIncludePaths.AddRange(new string[] {"ActorInventoryPluginEditor/Private"});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ActorInventoryPlugin",
				"UnrealEd"
			}
		);
	}
}