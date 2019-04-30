// (c) 2019 TinyGoose Ltd., All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SoccerTypeTarget : TargetRules
{
	public SoccerTypeTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "SoccerType" } );
	}
}
