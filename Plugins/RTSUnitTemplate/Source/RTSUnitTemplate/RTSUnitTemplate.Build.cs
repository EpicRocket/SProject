// Copyright 2022 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

using UnrealBuildTool;

public class RTSUnitTemplate : ModuleRules
{
	public RTSUnitTemplate(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		// Core dependencies
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTags" });

		// Ai dependencies
		PublicDependencyModuleNames.AddRange(new string[] { "AIModule" });

		// Hud dependencies
		PublicDependencyModuleNames.AddRange(new string[] { "HeadMountedDisplay", "NavigationSystem" });

		// Widget dependencies
		PublicDependencyModuleNames.AddRange(new string[] { "UMG", "MoviePlayer" });
		
	}
}