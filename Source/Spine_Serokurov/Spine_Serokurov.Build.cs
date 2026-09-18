// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Spine_Serokurov : ModuleRules
{
	public Spine_Serokurov(ReadOnlyTargetRules Target) : base(Target)
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
			"Spine_Serokurov",
			"Spine_Serokurov/Variant_Platforming",
			"Spine_Serokurov/Variant_Platforming/Animation",
			"Spine_Serokurov/Variant_Combat",
			"Spine_Serokurov/Variant_Combat/AI",
			"Spine_Serokurov/Variant_Combat/Animation",
			"Spine_Serokurov/Variant_Combat/Gameplay",
			"Spine_Serokurov/Variant_Combat/Interfaces",
			"Spine_Serokurov/Variant_Combat/UI",
			"Spine_Serokurov/Variant_SideScrolling",
			"Spine_Serokurov/Variant_SideScrolling/AI",
			"Spine_Serokurov/Variant_SideScrolling/Gameplay",
			"Spine_Serokurov/Variant_SideScrolling/Interfaces",
			"Spine_Serokurov/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
