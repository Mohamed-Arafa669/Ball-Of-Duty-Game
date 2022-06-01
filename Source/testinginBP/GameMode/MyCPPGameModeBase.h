// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "MyCPPGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API AMyCPPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMyCPPGameModeBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyGame")
		TSubclassOf<class ACharacter> FirstPawn; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyGame")
		TSubclassOf<class ACharacter> SecondPawn; 

	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //Replication

	//TODO : TEAMS 

	//void PostLogin(APlayerController* NewPlayer) override;

	//AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	//bool ShouldSpawnAtStartSpot(AController* Player) override { return false; };



protected:

	UPROPERTY (Replicated)
		TSubclassOf<class ACharacter> CurrentPawnToAssign;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
};
