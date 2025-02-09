using UnrealBuildTool;
 
public class MounteaAdvancedInventorySystemDeveloper : ModuleRules
{
	public MounteaAdvancedInventorySystemDeveloper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"BlueprintGraph"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Projects",
				"Engine",
				"Slate",
				"SlateCore", 
				"MounteaAdvancedInventorySystem", 
			}
		);

		if (Target.bBuildEditor == true) 
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"EditorStyle",
					"BlueprintGraph",
					"ApplicationCore",
				}
			);
		}
	}
}