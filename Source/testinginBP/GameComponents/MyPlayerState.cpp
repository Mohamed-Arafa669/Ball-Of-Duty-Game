// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"
#include "Net/UnrealNetwork.h" //Replication
#include "testinginBP/GameComponents/Teams.h"
#include "testinginBP/Character/CPPTestCharacter.h"
#include "testinginBP/PlayerController/CPPPlayerController.h"
#include "testinginBP/GameComponents/CombatComponent.h"

void AMyPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ACPPTestCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ACPPPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(Score);
		}
	}
	
}

void AMyPlayerState::AddToScore(float ScoreAmount)
{
	Score += ScoreAmount;
	/*Controller->SetHUDScore(ScoreAmount);*/

	Character = Character == nullptr ? Cast<ACPPTestCharacter>(GetPawn()) : Character;
	
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ACPPPlayerController>(combat->character->Controller) : Controller;

		if (Controller)
		{
			Controller->SetHUDScore(ScoreAmount);
		}
	}
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMyPlayerState, Team);
}