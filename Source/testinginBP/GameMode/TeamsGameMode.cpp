// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamsGameMode.h"
#include "testinginBP/GameState/MyGameState.h"
#include "testinginBP/GameComponents/MyPlayerState.h"
#include "Kismet/GameplayStatics.h"

void ATeamsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	AMyGameState* MyGameState = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));
	if (MyGameState)
	{
		AMyPlayerState* PlayerState = NewPlayer->GetPlayerState<AMyPlayerState>();
		if (PlayerState && PlayerState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (MyGameState->BlueTeam.Num() >= MyGameState->RedTeam.Num())
			{
				MyGameState->RedTeam.AddUnique(PlayerState);
				PlayerState->SetTeam(ETeam::ET_RedTeam);
			}
			else
			{
				MyGameState->BlueTeam.AddUnique(PlayerState);
				PlayerState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
	}
}

void ATeamsGameMode::Logout(AController* Exiting)
{
	AMyGameState* MyGameState = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));
	AMyPlayerState* PlayerState = Exiting->GetPlayerState<AMyPlayerState>();
	if (MyGameState && PlayerState)
	{
		if (MyGameState->RedTeam.Contains(PlayerState))
		{
			MyGameState->RedTeam.Remove(PlayerState);
		}
		if (MyGameState->BlueTeam.Contains(PlayerState))
		{
			MyGameState->BlueTeam.Remove(PlayerState);
		}
	}

}

void ATeamsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	AMyGameState* MyGameState = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));
	if (MyGameState)
	{
		for (auto PState : MyGameState->PlayerArray)
		{
			AMyPlayerState* PlayerState = Cast<AMyPlayerState>(PState.Get());
			if (PlayerState && PlayerState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (MyGameState->BlueTeam.Num() >= MyGameState->RedTeam.Num())
				{
					MyGameState->RedTeam.AddUnique(PlayerState);
					PlayerState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					MyGameState->BlueTeam.AddUnique(PlayerState);
					PlayerState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}
