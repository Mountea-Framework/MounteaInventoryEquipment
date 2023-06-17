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
				"KismetCompiler"
			}
		);
		
		PrivateDependencyModuleNames.AddRange
		(
			new string[]
			{
				"MounteaInventoryEquipment",
				//...
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