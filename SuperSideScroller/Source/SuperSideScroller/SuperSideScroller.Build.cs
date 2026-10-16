// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SuperSideScroller : ModuleRules
{
	public SuperSideScroller(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"SuperSideScroller",
			"SuperSideScroller/Variant_Platforming",
			"SuperSideScroller/Variant_Platforming/Animation",
			"SuperSideScroller/Variant_Combat",
			"SuperSideScroller/Variant_Combat/AI",
			"SuperSideScroller/Variant_Combat/Animation",
			"SuperSideScroller/Variant_Combat/Gameplay",
			"SuperSideScroller/Variant_Combat/Interfaces",
			"SuperSideScroller/Variant_Combat/UI",
			"SuperSideScroller/Variant_SideScrolling",
			"SuperSideScroller/Variant_SideScrolling/AI",
			"SuperSideScroller/Variant_SideScrolling/Gameplay",
			"SuperSideScroller/Variant_SideScrolling/Interfaces",
			"SuperSideScroller/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
