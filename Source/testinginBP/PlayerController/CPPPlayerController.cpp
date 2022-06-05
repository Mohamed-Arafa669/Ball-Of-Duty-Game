// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPPlayerController.h"
#include "testinginBP/HUD/GameHUD.h"
#include "testinginBP/HUD/CharacterOverlays.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "testinginBP/GameMode/MyGameMode.h"
#include "testinginBP/HUD/Announcement.h"


void ACPPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GameHUD = Cast<AGameHUD>(GetHUD());

	ServerCheckMatchState();

}


void ACPPPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (GameHUD && GameHUD->CharacterOverlay)
		{
			CharacterOverlay = GameHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
			}
		}
	}
}

void ACPPPlayerController::ServerCheckMatchState_Implementation()
{
	AMyGameMode* GameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();

		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, LevelStartingTime);

		
	}
}
void ACPPPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	StartingTime = LevelStartingTime;
	MatchState = StateOfMatch; 
	OnMatchStateSet(MatchState);

	if (GameHUD && MatchState == MatchState::WaitingToStart)
	{
		GameHUD->AddAnnouncement();
	}
}


void ACPPPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();

	CheckTimeSync(DeltaTime);
	PollInit();
}

float ACPPPlayerController::GetRealTime()
{
	return GetWorld()->GetTimeSeconds();
}

void ACPPPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPPPlayerController, MatchState);
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
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = CurrentHealth;
		HUDMaxHealth = MaxHealth;
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
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
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
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;

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

void ACPPPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetHUD()) : GameHUD;
	bool bHUDValidations = GameHUD &&
		GameHUD->Announcement &&
		GameHUD->Announcement->WarmupTime;

	if (bHUDValidations)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		GameHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void ACPPPlayerController::SetHUDTime()
{
	float MatchRemainingTime = MatchTime - GetServerTime();

	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchRemainingTime + LevelStartingTime;
	}

	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
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

void ACPPPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ACPPPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ACPPPlayerController::HandleMatchHasStarted()
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetHUD()) : GameHUD;
	if (GameHUD)
	{
		GameHUD->AddCharacterOverlay();
		if (GameHUD->Announcement)
		{
			GameHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ACPPPlayerController::HandleCooldown()
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetHUD()) : GameHUD;
	if (GameHUD)
	{
		GameHUD->CharacterOverlay->RemoveFromParent();
		if (GameHUD->Announcement)
		{
			GameHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

