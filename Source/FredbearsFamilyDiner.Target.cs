// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class FredbearsFamilyDinerTarget : TargetRules
{
    public FredbearsFamilyDinerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V4;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
        ExtraModuleNames.Add("FredbearsFamilyDiner");
    }
}
