// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"

#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/GameMode.h"
#include "../GameComponents/MyPlayerStart.h"
#include "testinginBP/Character/CPPTestCharacter.h"
#include "testinginBP/GameComponents/SpawnPoint.h"
#include "testinginBP/GameComponents/MyPlayerState.h"
#include "testinginBP/PlayerController/CPPPlayerController.h"
#include "TimerManager.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

AMyGameMode::AMyGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bDelayedStart = true;
	PlayerStateClass = AMyPlayerState::StaticClass();
	DefaultSpawnLocation = FVector(500.002479, -830.000000, 232.001526);
}

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = CurrentTime();

	UClass* SpawnPointClass = ASpawnPoint::StaticClass();
	for (TActorIterator<AActor> Point(GetWorld(), SpawnPointClass); Point; ++Point)
	{
		SpawnPoints.Add(Cast<ASpawnPoint>(*Point));
	}
}

void AMyGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - CurrentTime() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
			
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - CurrentTime() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
}

void AMyGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACPPPlayerController* MyPlayer = Cast<ACPPPlayerController>(*It);
		if (MyPlayer)
		{
			MyPlayer->OnMatchStateSet(MatchState);
		}
	}

}


void AMyGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyGameMode, CurrentPawnToAssign);
}

void AMyGameMode::PlayerEliminated(class ACPPTestCharacter* ElimmedCharacter, class ACPPPlayerController* VictimController, class APlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
	AMyPlayerState* AttackerPlayerState = AttackerController ? Cast<AMyPlayerState>(AttackerController->PlayerState) : nullptr;
	AMyPlayerState* VictimPlayerState = VictimController ? Cast<AMyPlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.0f);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Knocked();
	}
}


UClass* AMyGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	

	if (CurrentPawnToAssign)
	{
		
		if (FirstPawn != nullptr && SecondPawn != nullptr)
		{
			if (CurrentPawnToAssign == FirstPawn)
			{
				CurrentPawnToAssign = SecondPawn;
			}

			else
			{
				CurrentPawnToAssign = FirstPawn;

			}

		}
	}
	else
	{
		if (FirstPawn != nullptr && SecondPawn != nullptr)
		{
			CurrentPawnToAssign = (true) ? FirstPawn : SecondPawn;
		}
	}
	
	return CurrentPawnToAssign;

}

class ASpawnPoint* AMyGameMode::GetSpawnPoint()
{
	for (int32 i = 0; i < SpawnPoints.Num(); i++)
	{
		int32 Point = FMath::RandRange(0, SpawnPoints.Num() - 1);

		if (SpawnPoints[Point])
		{
			return SpawnPoints[Point];
		}
	}
	return nullptr;
}

void AMyGameMode::Spawn(AController* Controller)
{
	if (ASpawnPoint* SpawnPoint = GetSpawnPoint()) {

		FVector Location = SpawnPoint->GetActorLocation();
		FRotator Rotation = SpawnPoint->GetActorRotation();
		if (APawn* Pawn = GetWorld()->SpawnActor<APawn>(Controller->GetPawn()->GetClass(), Location, Rotation))
		{
			Controller->Possess(Pawn);
		}
	}
	else
	{
		FVector Location = DefaultSpawnLocation;
		FRotator Rotation = FRotator::ZeroRotator;
		if (APawn* Pawn = GetWorld()->SpawnActor<APawn>(Controller->GetPawn()->GetClass(), Location, Rotation))
		{
			Controller->Possess(Pawn);
		}
	}
}

void AMyGameMode::Respawn(AController* Controller)
{
	if (Controller) {

		if (GetLocalRole() == ROLE_Authority)
		{

			FTimerDelegate RespawnDelegate;
			RespawnDelegate.BindUFunction(this, FName("Spawn"), Controller);

			GetWorld()->GetTimerManager().SetTimer(RespawnHandle, RespawnDelegate, RespawnTime, false);


			if (ACPPTestCharacter* MyChar = Cast<ACPPTestCharacter>(Controller->GetCharacter()))
			{
				MyChar->ClientRespawnCountDown(5);
			}
		}
	}
}

float AMyGameMode::CurrentTime()
{
	return GetWorld()->GetTimeSeconds();
}

