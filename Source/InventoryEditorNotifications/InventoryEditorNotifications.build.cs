using UnrealBuildTool;
 
public class InventoryEditorNotifications : ModuleRules
{
	public InventoryEditorNotifications(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;
		
		PrecompileForTargets = PrecompileTargetsType.None;
		bPrecompile = false;
		bUsePrecompiled = false;

		PublicDependencyModuleNames.AddRange
		(new string[]
			{
				"Core", 
				"CoreUObject", 
				"Engine",
				"Slate", 
				"SlateCore", 
				"Projects"
			}
		);	
	}
}