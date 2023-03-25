using UnrealBuildTool;
using System.IO;

public class XLNT : ModuleRules
{
	public XLNT(ReadOnlyTargetRules Target) : base(Target)
	{
        if (Target.Platform != UnrealTargetPlatform.Win64)
		{
			// Currently we support only Win64 llvm builds.
			return;
		}
        
		Type = ModuleType.External;

		// Path to the xlnt headers
        PublicIncludePaths.AddRange(
			new string[]
            {
				Path.Combine(ModuleDirectory, "include"),
		    }
        );

		// Add the xlnt library
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "xlnt.lib"));
	}
}