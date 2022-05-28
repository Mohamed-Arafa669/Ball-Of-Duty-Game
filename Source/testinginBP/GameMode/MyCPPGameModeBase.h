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
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyGame")
		TSubclassOf<class ACharacter> FirstPawn; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyGame")
		TSubclassOf<class ACharacter> SecondPawn; 

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyGame")
		TSubclassOf<class ACharacter> ThirdPawn;*/

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyGame")
		TSubclassOf<class ACharacter> FourthPawn; */

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //Replication

	//TODO : TEAMS 

	//void PostLogin(APlayerController* NewPlayer) override;

	//AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	//bool ShouldSpawnAtStartSpot(AController* Player) override { return false; };

	void Respawn(AController* Controller);

protected:

	UPROPERTY (Replicated)
		TSubclassOf<class ACharacter> CurrentPawnToAssign;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	TArray<class ASpawnPoint*> SpawnPoints;
	FVector DefaultSpawnLocation;

	class ASpawnPoint* GetSpawnPoint();

	UFUNCTION()
	void Spawn(AController* Controller);

	FTimerHandle RespawnHandle;
public:
	UPROPERTY(EditAnywhere, Category = "Respawning")
		float RespawnTime = 4.f;

	void RespawnCountDown();

	UPROPERTY(BlueprintReadOnly, Replicated)
	int seconds = 5.f;

	UPROPERTY(BlueprintReadWrite)
	bool isDead;



	//RespawingWidget;
};
