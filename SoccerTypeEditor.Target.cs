// (c) 2019 TinyGoose Ltd., All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SoccerTypeEditorTarget : TargetRules
{
	public SoccerTypeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "SoccerType" } );
	}
}
