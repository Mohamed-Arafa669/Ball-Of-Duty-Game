// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPPlayerController.h"
#include "testinginBP/HUD/GameHUD.h"
#include "testinginBP/HUD/CharacterOverlays.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"


void ACPPPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	GameHUD = Cast<AGameHUD>(GetHUD());
	
}


void ACPPPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();

	CheckTimeSync(DeltaTime);
}

void ACPPPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetRealTime());
		TimeSyncRunningTime = 0.f;
	}
}


void ACPPPlayerController::SetHUDHealth(float CurrentHealth, float MaxHealth)
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetHUD()) : GameHUD;
	if (GameHUD && GameHUD->CharacterOverlay && GameHUD->CharacterOverlay->HealthBar)
	{
		const float HealthPercent = CurrentHealth / MaxHealth;
		GameHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
	}
}

void ACPPPlayerController::SetHUDScore(float Score)
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetHUD()) : GameHUD;
	bool bHUDValidations = GameHUD &&
		GameHUD->CharacterOverlay &&
		GameHUD->CharacterOverlay->ScoreAmount;
	
	if (bHUDValidations)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		GameHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}

}

void ACPPPlayerController::SetHUDDefeats(int32 Defeats)
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetHUD()) : GameHUD;
	bool bHUDValidations = GameHUD &&
		GameHUD->CharacterOverlay &&
		GameHUD->CharacterOverlay->DefeatsAmount;

	if (bHUDValidations)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"),Defeats);
		GameHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}

}

void ACPPPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetHUD()) : GameHUD;
	bool bHUDValidations = GameHUD &&
		GameHUD->CharacterOverlay &&
		GameHUD->CharacterOverlay->MatchCountdownText;

	if (bHUDValidations)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime/60.0f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"),Minutes,Seconds);
		GameHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void ACPPPlayerController::SetHUDTime()
{
	float MatchRemainingTime = MatchTime - GetServerTime();
	uint32 SecondsLeft = FMath::CeilToInt(MatchRemainingTime);

	if (CountdownInt != SecondsLeft)
	{
		SetHUDMatchCountdown(MatchTime - GetServerTime());
	}

	CountdownInt = SecondsLeft;
}

void ACPPPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetRealTime();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ACPPPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerRecievedClientRequest)
{
	float RoundTripTime = GetRealTime() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerRecievedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetRealTime();
}

float ACPPPlayerController::GetServerTime()
{
	if(HasAuthority())
	{
		return GetRealTime();
	}
	else
	{
		return GetRealTime() + ClientServerDelta;
	}
}

void ACPPPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetRealTime());
	}
}

float ACPPPlayerController::GetRealTime()
{
	return GetWorld()->GetTimeSeconds();
}

