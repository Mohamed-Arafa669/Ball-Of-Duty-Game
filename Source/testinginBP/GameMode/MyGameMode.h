// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MyGameMode.generated.h"

namespace MatchState
{
	extern TESTINGINBP_API const FName Cooldown;	// Match duration has been reached. Display the winner and begin the cooldown timer..
}
/**
 * 
 */
UCLASS()
class TESTINGINBP_API AMyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	
	AMyGameMode();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnBall")
	TArray<TSubclassOf<class ABallSpawner>> BallSpawner;

	UPROPERTY(EditAnywhere, Category = "SpawnBall")
		float NumerOfSecondBetweenSpawns = 1.0f;

	float SpawnTimer;

	int32 GetRandomLocation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyGame")
	TSubclassOf<class ACharacter> FirstPawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyGame")
	TSubclassOf<class ACharacter> SecondPawn;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //Replication

	virtual void PlayerEliminated(
		class ACPPTestCharacter* ElimmedCharacter,
		class ACPPPlayerController* VictimController,
		class APlayerController* AttackerController
	);

	void Respawn(AController* Controller);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
		float CooldownTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	float LevelStartingTime = 0.f;

private:

	float CountdownTime = 0.f;

protected:

	UPROPERTY(Replicated)
	TSubclassOf<class ACharacter> CurrentPawnToAssign;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	virtual void OnMatchStateSet() override;

	TArray<class ASpawnPoint*> SpawnPoints;

	FVector DefaultSpawnLocation;

	class ASpawnPoint* GetSpawnPoint();

	UFUNCTION()
	void Spawn(AController* Controller);

	float CurrentTime();

	FTimerHandle RespawnHandle;

public:
	
	UPROPERTY(EditAnywhere, Category = "Respawning")
	float RespawnTime = 3.f;

};
