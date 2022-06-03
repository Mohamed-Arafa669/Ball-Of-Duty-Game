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
	void SetHUDHealth(float CurrentHealth, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDTime();
	virtual void Tick(float DeltaTime) override;
	virtual float GetServerTime(); // Synced with server world clock
	virtual void ReceivedPlayer() override; // Sync with server clock as soon as possible
	 
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

	float MatchTime = 120.f;

	uint32 CountdownInt = 0;

protected:
	virtual void BeginPlay() override;
	//void SetHUDMatchCountdown(float CountdownTime);

};
