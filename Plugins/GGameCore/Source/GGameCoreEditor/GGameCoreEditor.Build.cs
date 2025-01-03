
using UnrealBuildTool;

public class GGameCoreEditor : ModuleRules
{
	public GGameCoreEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
		new string[]
			{
				"BlueprintGraph",
				"Core",
				"CoreUObject",
				"Engine",
				"KismetCompiler",
				"PropertyEditor",
				"UnrealEd",
			}
        );

        PrivateDependencyModuleNames.AddRange(
		new string[]
			{
				"GGameCore",
			}
        );
	}
}
