// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCPPGameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "../GameComponents/MyPlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "../GameComponents/MyPlayerStart.h"

AMyCPPGameModeBase::AMyCPPGameModeBase() 
{
PlayerStateClass = AMyPlayerState::StaticClass();
}

void AMyCPPGameModeBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyCPPGameModeBase, CurrentPawnToAssign);
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
