// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"
#include "testinginBP/Character/CPPTestCharacter.h"
#include "testinginBP/PlayerController/CPPPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "../GameComponents/MyPlayerState.h"
#include "GameFramework/GameMode.h"
#include "../GameComponents/MyPlayerStart.h"
#include "Kismet/GameplayStatics.h"

AMyGameMode::AMyGameMode()
{
	PlayerStateClass = AMyPlayerState::StaticClass();

}

void AMyGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyGameMode, CurrentPawnToAssign);
}

void AMyGameMode::PlayerEliminated(class ACPPTestCharacter* ElimmedCharacter, class ACPPPlayerController* VictimController, class ACPPPlayerController* AttackerController)
{
	/*AMyPlayerState* AttackerPlayerState = AttackerController ? Cast<AMyPlayerState>(AttackerController->PlayerState) : nullptr;
	AMyPlayerState* VictimPlayerState = VictimController ? Cast<AMyPlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState)
	{
		AttackerPlayerState->AddToScore(1.0f);
	}*/

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Knocked();
	}
}

void AMyGameMode::RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroyed();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
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