// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Pickups : ModuleRules
{
	public Pickups(ReadOnlyTargetRules Target) : base(Target)
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
			"Pickups",
			"Pickups/Variant_Platforming",
			"Pickups/Variant_Platforming/Animation",
			"Pickups/Variant_Combat",
			"Pickups/Variant_Combat/AI",
			"Pickups/Variant_Combat/Animation",
			"Pickups/Variant_Combat/Gameplay",
			"Pickups/Variant_Combat/Interfaces",
			"Pickups/Variant_Combat/UI",
			"Pickups/Variant_SideScrolling",
			"Pickups/Variant_SideScrolling/AI",
			"Pickups/Variant_SideScrolling/Gameplay",
			"Pickups/Variant_SideScrolling/Interfaces",
			"Pickups/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
