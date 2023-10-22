using UnrealBuildTool;
 
public class MounteaInventoryEquipmentDeveloper : ModuleRules
{
	public MounteaInventoryEquipmentDeveloper(ReadOnlyTargetRules Target) : base(Target)
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
				"Projects",
				
				"BlueprintGraph",
				"KismetCompiler",
				"UnrealEd",
				"ToolMenus"
			}
		);	
	}
}