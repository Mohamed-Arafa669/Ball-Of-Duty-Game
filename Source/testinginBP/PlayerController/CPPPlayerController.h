// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API ACPPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //Replication
	void SetHUDHealth(float CurrentHealth, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDTime();
	virtual void Tick(float DeltaTime) override;
	virtual float GetServerTime(); // Synced with server world clock
	virtual void ReceivedPlayer() override; // Sync with server clock as soon as possible
	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCooldown();
	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim);

	//////////////////////////////////////////////////////////////////////////
	///
	///	 Character Selection
	/// 

	UFUNCTION(Server, Reliable)
		void ServerSelectedCharacter(uint8 index);

	uint8 CharacterSelectIndex = -1;

	UPROPERTY(EditAnywhere, Category = "CharacterSelection")
		TSubclassOf<class UUserWidget> CharacterSelectionClass;

	UPROPERTY()
		class UCharacterSelection* CharacterSelection;

	void CreateCharacterSelectMenu();
	/// <summary>
	/// Time Sync Between Client and Server
	/// </summary>
	
	// Request the current server time by passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
		void ServerRequestServerTime(float TimeOfClientRequest);

	//Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
		void ClientReportServerTime(float TimeOfClientRequest, float TimeServerRecievedClientRequest);

	float ClientServerDelta = 0.f; //difference between client and server time

	UPROPERTY(EditAnywhere , Category = Time)
	float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);

	float GetRealTime();
	UPROPERTY()
	class AGameHUD* GameHUD;

	float LevelStartingTime = 0.f;

	float MatchTime = 0.f;

	float WarmupTime = 0.f;

	float CooldownTime = 0.f;

	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();


	UPROPERTY()
		class UCharacterOverlays* CharacterOverlay;

	UPROPERTY()
		class AMyGameMode* GameMode;

	UPROPERTY()
		class ACPPTestCharacter* MyCharacter;

	UPROPERTY()
		class AMyPlayerState* MyPlayerState;

	UPROPERTY()
		class AMyGameState* MyGameState;

	UPROPERTY()
		TArray<AMyPlayerState*> TopPlayers;


	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;

protected:
	virtual void BeginPlay() override;
	void PollInit();
	virtual void SetupInputComponent() override;
	void ShowReturnToMainMenu();

	UFUNCTION(Server, Reliable)
		void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
		void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

	UFUNCTION(Client, Reliable)
	void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);

	void OnPossess(APawn* InPawn) override;

private:
	
	///
	/// Return To Main Menu
	/// 

	UPROPERTY(EditAnywhere, Category = HUD)
		TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;

	UPROPERTY()
	class UInGameMenu* ReturnToMainMenu;

	bool bIsInMenu = false;
};
