using UnrealBuildTool;
using System.IO;
using System;

public class OpenXLSX : ModuleRules
{
    public OpenXLSX(ReadOnlyTargetRules Target) : base(Target)
    {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
		    var ThirdPartyPath = Path.Combine(ModuleDirectory, "../ThirdParty", Target.Platform.ToString());

		    PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "include"));

            if (Target.Configuration == UnrealTargetConfiguration.Debug && Target.bDebugBuildsActuallyUseDebugCRT)
            {
                PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Debug", "OpenXLSXd.lib"));
            }
            else
            {
                PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "lib", "Release", "OpenXLSX.lib"));
            }

            PublicDefinitions.Add("WIN32_LEAN_AND_MEAN");
        }
        
        PublicDependencyModuleNames.AddRange(
			new [] 
			{
				"Core",
				"CoreUObject",
				"Engine",
			}
		);
    }
}
