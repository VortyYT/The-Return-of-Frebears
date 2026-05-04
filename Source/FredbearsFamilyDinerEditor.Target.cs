// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class FredbearsFamilyDinerEditorTarget : TargetRules
{
    public FredbearsFamilyDinerEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V4;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
        ExtraModuleNames.Add("FredbearsFamilyDiner");
    }
}
