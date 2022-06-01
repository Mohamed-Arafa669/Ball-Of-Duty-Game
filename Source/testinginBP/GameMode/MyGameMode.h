// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API AMyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	
	AMyGameMode();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyGame")
		TSubclassOf<class ACharacter> FirstPawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyGame")
		TSubclassOf<class ACharacter> SecondPawn;



	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //Replication

	//TODO : TEAMS 

	//void PostLogin(APlayerController* NewPlayer) override;

	//AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	//bool ShouldSpawnAtStartSpot(AController* Player) override { return false; };
	// 
	// 
	virtual void PlayerEliminated(class ACPPTestCharacter* ElimmedCharacter, class ACPPPlayerController* VictimController, class ACPPPlayerController* AttackerController);

protected:

	UPROPERTY(Replicated)
		TSubclassOf<class ACharacter> CurrentPawnToAssign;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

};
