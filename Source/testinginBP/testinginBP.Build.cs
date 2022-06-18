// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class testinginBP : ModuleRules
{
	public testinginBP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "LockOnTarget" ,"MultiplayerSessions", "OnlineSubsystem","OnlineSubsystemSteam", "Niagara", "GameplayCameras"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.AddRange(new string[] { "S:\\Game Dev\\UE_5.0\\Engine\\Plugins\\FX\\Niagara\\Source\\Niagara\\Public"});


		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
