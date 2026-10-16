// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Dodgeball : ModuleRules
{
	public Dodgeball(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Dodgeball",
			"Dodgeball/Variant_Platforming",
			"Dodgeball/Variant_Platforming/Animation",
			"Dodgeball/Variant_Combat",
			"Dodgeball/Variant_Combat/AI",
			"Dodgeball/Variant_Combat/Animation",
			"Dodgeball/Variant_Combat/Gameplay",
			"Dodgeball/Variant_Combat/Interfaces",
			"Dodgeball/Variant_Combat/UI",
			"Dodgeball/Variant_SideScrolling",
			"Dodgeball/Variant_SideScrolling/AI",
			"Dodgeball/Variant_SideScrolling/Gameplay",
			"Dodgeball/Variant_SideScrolling/Interfaces",
			"Dodgeball/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
