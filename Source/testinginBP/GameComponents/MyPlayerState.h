// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "testinginBP/GameComponents/Teams.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	//UPROPERTY(Replicated)
	//	bool bTeamB;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //Replication

	/// <summary>
	/// Replication notifiers
	/// </summary>
	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();
//
	void AddToScore(float ScoreAmount);

	void AddToDefeats(float DefeatsAmount);

//
//
private:

	UPROPERTY()
	class ACPPTestCharacter* Character;

	UPROPERTY()
	class ACPPPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;
//
//	UPROPERTY(VisibleAnywhere)
//		class UCombatComponent* combat;
//	UPROPERTY(Replicated)
//	ETeam Team = ETeam::ET_NoTeam;
//
//public:
//	FORCEINLINE ETeam GetTeam() const { return Team; }
//	FORCEINLINE void SetTeam(ETeam TeamToSet) { Team = TeamToSet; }
};
