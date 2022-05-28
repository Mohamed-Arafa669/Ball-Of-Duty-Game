// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCPPGameModeBase.h"

#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "../GameComponents/MyPlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "../GameComponents/MyPlayerStart.h"
#include "testinginBP/Character/CPPTestCharacter.h"
#include "testinginBP/GameComponents/SpawnPoint.h"
#include "TimerManager.h"

AMyCPPGameModeBase::AMyCPPGameModeBase() 
{
	PlayerStateClass = AMyPlayerState::StaticClass();

	DefaultSpawnLocation = FVector(500.002479, -830.000000, 232.001526);
}

void AMyCPPGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UClass* SpawnPointClass = ASpawnPoint::StaticClass();
	for (TActorIterator<AActor> Point (GetWorld(),SpawnPointClass); Point; ++Point)
	{
		SpawnPoints.Add(Cast<ASpawnPoint>(*Point));
	}

}

void AMyCPPGameModeBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyCPPGameModeBase, CurrentPawnToAssign);
	DOREPLIFETIME(AMyCPPGameModeBase, seconds);

}



UClass* AMyCPPGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (CurrentPawnToAssign)
	{
		if (FirstPawn != nullptr && SecondPawn != nullptr )
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
		if (FirstPawn != nullptr && SecondPawn != nullptr )
		{
			CurrentPawnToAssign = (true) ? FirstPawn : SecondPawn;
		}
	

	
	}
	return CurrentPawnToAssign;
}

ASpawnPoint* AMyCPPGameModeBase::GetSpawnPoint()
{
	for (int32 i = 0; i < SpawnPoints.Num(); i++)
	{
		int32 Point = FMath::RandRange(0, SpawnPoints.Num() - 1);

		if(SpawnPoints[Point])
		{
			return SpawnPoints[Point];
		}
	}
	return nullptr;
}

void AMyCPPGameModeBase::Spawn(AController* Controller)
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

void AMyCPPGameModeBase::RespawnCountDown()
{
	
	if (seconds != 0)
	{
		seconds--;
		FTimerHandle RespawnCountHandle;
		GetWorldTimerManager().SetTimer(RespawnCountHandle, this, &ThisClass::RespawnCountDown, 1.f, false, 1.f);

	} else
	{
		isDead = false;
		seconds = 5;
	}
}


void AMyCPPGameModeBase::Respawn(AController* Controller)
{

	if (Controller) {
		isDead = true;
		if (GetLocalRole() == ROLE_Authority)
		{



			RespawnCountDown();

			FTimerDelegate RespawnDelegate;
			RespawnDelegate.BindUFunction(this, FName("Spawn"), Controller);

			GetWorld()->GetTimerManager().SetTimer(RespawnHandle, RespawnDelegate, RespawnTime, false);

			FString HitMessage = FString::Printf(TEXT("Dead"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, HitMessage);
		}
	}
}
//TODO : Teams 

//void AMyCPPGameModeBase::PostLogin(APlayerController* NewPlayer)
//{
//	Super::PostLogin(NewPlayer);
//	if (NewPlayer)
//	{
//		AMyPlayerState* PS = Cast<AMyPlayerState>(NewPlayer->PlayerState);
//		if (PS && GameState)
//		{
//			uint8 NumTeamA = 0;
//			uint8 NumTeamB = 0;
//			for (APlayerState* It : GameState->PlayerArray)
//			{
//				AMyPlayerState* OtherPS = Cast<AMyPlayerState>(It);
//				if (OtherPS)
//				{
//					if (OtherPS->bTeamB)
//					{
//						NumTeamB++;
//					}
//					else
//					{
//						NumTeamA++;
//
//					}
//				}
//			}
//			if (NumTeamA > NumTeamB)
//			{
//				PS->bTeamB = true;
//			}
//		}
//	}
//}
//
//AActor* AMyCPPGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
//{
//	if (Player)
//	{
//		AMyPlayerState* PS = Cast<AMyPlayerState>(Player->PlayerState);
//		if (PS)
//		{
//			TArray<AMyPlayerState*> Starts;
//			for (TActorIterator<AMyPlayerStart> StartItr(GetWorld()); StartItr; ++StartItr)
//			{
//				if (StartItr->bTeamB == PS->bTeamB)
//				{
//					Starts.Add(*StartItr);
//				}
//			}
//			return Starts[FMath::RandRange(0, Starts.Num() - 1)];
//		}
//	}
//	return NULL;		
//}