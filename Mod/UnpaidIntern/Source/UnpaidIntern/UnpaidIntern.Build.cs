// Copyright (c) 2026 Jack Reason. MIT License.
// TODO(M1): Open inside the Satisfactory Modding Starter Project and regenerate VS files before compiling.
// Docs: https://docs.ficsit.app/satisfactory-modding/latest/Development/Cpp/setup.html

using UnrealBuildTool;
using System.IO;
using System;

public class UnpaidIntern : ModuleRules
{
	public UnpaidIntern(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bLegacyParentIncludePaths = false;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject",
			"Engine",
			"DeveloperSettings",
			"PhysicsCore",
			"InputCore",
			"OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemNull",
			"SignificanceManager",
			"GeometryCollectionEngine",
			"ChaosVehiclesCore", "ChaosVehicles", "ChaosSolverEngine",
			"AnimGraphRuntime",
			"AkAudio",
			"AssetRegistry",
			"NavigationSystem",
			"ReplicationGraph",
			"AIModule",
			"GameplayTasks",
			"SlateCore", "Slate", "UMG",
			"RenderCore",
			"CinematicCamera",
			"Foliage",
			"Niagara",
			"EnhancedInput",
			"GameplayTags",
			"Json", "JsonUtilities",
			"NetCore",
			"GameplayTasks",
			"HTTP",
			"Sockets", "Networking",
		});

		PublicDependencyModuleNames.AddRange(new string[] {
			"FactoryGame",
			"SML",
		});
	}
}
