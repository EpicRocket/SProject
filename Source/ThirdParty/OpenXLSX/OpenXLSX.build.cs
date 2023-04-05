using UnrealBuildTool;
using System.IO;
using System;

public class OpenXLSX : ModuleRules
{
    public OpenXLSX(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

            if (Target.Configuration == UnrealTargetConfiguration.Debug && Target.bDebugBuildsActuallyUseDebugCRT)
            {
                PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "Debug", "OpenXLSXd.lib"));
            }
            else
            {
                PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "Release", "OpenXLSX.lib"));
            }
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
