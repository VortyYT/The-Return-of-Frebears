// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

using UnrealBuildTool;

public class FredbearsFamilyDiner : ModuleRules
{
    public FredbearsFamilyDiner(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "AIModule",
            "NavigationSystem",
            "GameplayTasks",
            "UMG",
            "Slate",
            "SlateCore",
            "Niagara"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "AudioMixer",
            "PhysicsCore"
        });

        // Enable IWYU (Include What You Use) for faster compilation
        bEnforceIWYU = true;

        // Enable exceptions for robust error handling
        bEnableExceptions = true;
    }
}
