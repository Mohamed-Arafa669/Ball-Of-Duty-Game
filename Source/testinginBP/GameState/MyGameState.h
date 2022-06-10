// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API AMyGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //Replication
	void UpdateTopScore(class AMyPlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<class AMyPlayerState*> TopScoringPlayers;

private:

	float TopScore = 0.f;
	
};
