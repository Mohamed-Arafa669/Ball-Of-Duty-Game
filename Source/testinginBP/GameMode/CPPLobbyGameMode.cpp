// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPLobbyGameMode.h"
#include "GameFramework/GameStateBase.h"


void ACPPLobbyGameMode::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);

	int32 numberOfPlayers = GameState.Get()->PlayerArray.Num();

	if (numberOfPlayers == 2)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			bUseSeamlessTravel = true;
			world->ServerTravel(FString("/Game/ThirdPersonBP/Maps/GameMap?listen"));
		}
	}
}
