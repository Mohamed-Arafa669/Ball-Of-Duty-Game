// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"
#include "Net/UnrealNetwork.h" //Replication
#include "testinginBP/GameComponents/Teams.h"
#include "testinginBP/Character/CPPTestCharacter.h"
#include "testinginBP/PlayerController/CPPPlayerController.h"
#include "testinginBP/GameComponents/CombatComponent.h"

//void AMyPlayerState::AddToScore(float ScoreAmount)
//{
//	ScoreAmount = 5.0f;
//
//	sCharacter = /*sCharacter == nullptr ?*/ Cast<ACPPTestCharacter>(GetPawn()) /*: sCharacter*/;
//
//	if (sCharacter)
//	{
//		sController = sController == nullptr ? Cast<ACPPPlayerController>(sCharacter->Controller) : sController;
//
//		if (sController)
//		{
//			sController->SetHUDScore(ScoreAmount);
//		}
//	}
//}
//
//void AMyPlayerState::OnRep_Score()
//{
//	Super::OnRep_Score();
//
//	sCharacter = sCharacter == nullptr ? Cast<ACPPTestCharacter>(GetPawn()) : sCharacter;
//	if (sCharacter)
//	{
//		sController = sController == nullptr ? Cast<ACPPPlayerController>(sCharacter->Controller) : sController;
//		if (sController)
//		{
//			sController->SetHUDScore(Score);
//		}
//	}
//	
//}
//
//
//
//void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	
//	DOREPLIFETIME(AMyPlayerState, Team);
//}