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
#include "testinginBP/Character/CPPTestCharacter.h"
#include "testinginBP/HUD/CharacterSelection.h"
#include "testinginBP/GameComponents/CombatComponent.h"
#include "testinginBP/GameState/MyGameState.h"
#include "testinginBP/GameComponents/MyPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "testinginBP/HUD/InGameMenu.h"

void ACPPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GameHUD = Cast<AGameHUD>(GetHUD());

	//GameHUD->AddCharacterSelect();
	//CreateCharacterSelectMenu();

	ServerCheckMatchState();

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

void ACPPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr) return;

	InputComponent->BindAction("Quit", IE_Pressed, this, &ACPPPlayerController::ShowReturnToMainMenu);  
	
}

void ACPPPlayerController::ShowReturnToMainMenu()
{
	if (ReturnToMainMenuWidget == nullptr) return;
	if (ReturnToMainMenu == nullptr)
	{
		ReturnToMainMenu = CreateWidget<UInGameMenu>(this, ReturnToMainMenuWidget);
	}
	if (ReturnToMainMenu)
	{
		bIsInMenu = !bIsInMenu;

		if (bIsInMenu)
		{
			ReturnToMainMenu->MenuSetup();
		}
		else
		{
			ReturnToMainMenu->MenuTearDown();
		}
	}
}

void ACPPPlayerController::ServerCheckMatchState_Implementation()
{
	GameMode = GameMode == nullptr ? Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this)) : GameMode;
	//AMyGameMode* GameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();

		ClientJoinMidgame(MatchState, WarmupTime, MatchTime,CooldownTime, LevelStartingTime);	
	}
}
void ACPPPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch; 
	OnMatchStateSet(MatchState);

	if (GameHUD && MatchState == MatchState::WaitingToStart)
	{
		GameHUD->AddAnnouncement();
		GameHUD->AddCharacterSelect();

	}
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
		if (CooldownTime < 0.f)
		{
			GameHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;

		}
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
		if (CountdownTime < 0.f)
		{
			GameHUD->Announcement->WarmupTime->SetText(FText());
			return;

		}
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
	if (MatchState == MatchState::WaitingToStart)	TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress)	TimeLeft = WarmupTime + MatchRemainingTime + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown)	TimeLeft = WarmupTime + MatchRemainingTime + LevelStartingTime + CooldownTime;
	
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (HasAuthority())
	{ 
		GameMode = GameMode == nullptr ? Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this)) : GameMode;
		if (GameMode)
		{
			SecondsLeft = FMath::CeilToInt(GameMode->GetCountdownTime() + LevelStartingTime);
		}

	}

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
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


void ACPPPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	MyCharacter = MyCharacter == nullptr ? Cast<ACPPTestCharacter>(InPawn) : MyCharacter;
	//ACPPTestCharacter* MyCharacter = Cast<ACPPTestCharacter>(InPawn);
	if (MyCharacter)
	{
		SetHUDHealth(MyCharacter->GetCurrentHealth(), MyCharacter->GetMaxHealth());
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
			GameHUD->CharacterSelection->CloseWidget();
		}
	}
}

void ACPPPlayerController::HandleCooldown()
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetHUD()) : GameHUD;
	if (GameHUD)
	{
		GameHUD->CharacterOverlay->RemoveFromParent();

		bool bHUDValidations = GameHUD->Announcement &&
			GameHUD->Announcement->AnnouncementText &&
			GameHUD->Announcement->InfoText;
		if (bHUDValidations)
		{
			GameHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("Returning to Main Menu In:");
			GameHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			/// <summary>
			/// THE SCOREBOARD DISPLAY
			/// </summary>
			//AMyGameState* GameState = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));
			//AMyPlayerState* MPlayerState = GetPlayerState<AMyPlayerState>();
			MyGameState = MyGameState == nullptr ? Cast<AMyGameState>(UGameplayStatics::GetGameState(this)) : MyGameState;
			MyPlayerState = MyPlayerState == nullptr ? GetPlayerState<AMyPlayerState>() : MyPlayerState;

			TopPlayers = MyGameState->TopScoringPlayers;

			if (MyGameState && MyPlayerState)
			{

				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("It's A TIE");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == MyPlayerState)
				{
					InfoTextString = FString("YOU ARE THE MASTER OF KNOCKS");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("THE MASTER OF KNOCKS is: \n%s "), *TopPlayers[0]->GetPlayerName());  
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win:\n");
					for (auto TiedPlayers : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayers->GetPlayerName()));
					}
				}
				GameHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));

			}
		}
	}
	/*MyCharacter = MyCharacter == nullptr ? Cast<ACPPTestCharacter>(GetPawn()) : MyCharacter;
	if (MyCharacter && MyCharacter->GetCombat() )
	{
		MyCharacter->bDisableGameplay = true;
		MyCharacter->GetCombat()->ThrowButtonPressed(false);
	}*/
}

void ACPPPlayerController::BroadcastElim(APlayerState* Attacker, APlayerState* Victim)
{
	ClientElimAnnouncement(Attacker, Victim);
}

void ACPPPlayerController::ServerSelectedCharacter_Implementation(uint8 index)
{
	CharacterSelectIndex = index;
	ServerRestartPlayer();

}

void ACPPPlayerController::ClientElimAnnouncement_Implementation(APlayerState* Attacker, APlayerState* Victim)
{
	APlayerState* Self = GetPlayerState<APlayerState>();
	if (Attacker && Victim && Self)
	{
		GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetHUD()) : GameHUD;
		if (GameHUD)
		{
			if (Attacker == Self && Victim != Self)
			{
				GameHUD->AddElimAnnouncement("You", Victim->GetPlayerName());
				return;
			}
			if (Victim == Self && Attacker != Self)
			{
				GameHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "You");
				return;
			}
			if (Attacker == Self && Victim == Self)
			{
				GameHUD->AddElimAnnouncement("You", "Yourself");
				return;
			}
			if (Attacker == Victim && Attacker != Self)
			{
				GameHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "Themselves");
				return;
			}
			GameHUD->AddElimAnnouncement(Attacker->GetPlayerName(), Victim->GetPlayerName());
		}

	}
}

//void ACPPPlayerController::CreateCharacterSelectMenu()
//{
//	if (!ensure(CharacterSelectionClass != nullptr))
//	{
//		return;
//	}
//	CharacterSelection = CreateWidget<UCharacterSelection>(this, CharacterSelectionClass);
//
//	if (!ensure(CharacterSelection != nullptr))
//	{
//		return;
//	}
//
//	CharacterSelection->Setup();
//}
