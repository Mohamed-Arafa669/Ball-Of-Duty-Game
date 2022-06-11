// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"
#include "Net/UnrealNetwork.h" //Replication
#include "testinginBP/GameComponents/Teams.h"
#include "testinginBP/Character/CPPTestCharacter.h"
#include "testinginBP/PlayerController/CPPPlayerController.h"
#include "testinginBP/GameComponents/CombatComponent.h"

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyPlayerState, Defeats);
	DOREPLIFETIME(AMyPlayerState, Team);


}
void AMyPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<ACPPTestCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ACPPPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}


void AMyPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ACPPTestCharacter>(GetPawn()) : Character;
	if (Character)
	{
	Controller = Controller == nullptr ? Cast<ACPPPlayerController>(Character->Controller) : Controller;
	if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
	
}

void AMyPlayerState::AddToDefeats(float DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<ACPPTestCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ACPPPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}


void AMyPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<ACPPTestCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ACPPPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}
void AMyPlayerState::OnRep_Team()
{

}

void AMyPlayerState::SetTeam(ETeam TeamToSet)
{
}

