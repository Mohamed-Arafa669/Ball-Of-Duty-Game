// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "BallSpawner.generated.h"

UCLASS()
class TESTINGINBP_API ABallSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABallSpawner();

	UFUNCTION()
		void SpawnActor();

	UFUNCTION()
		void SpawnCoolDown();

	UPROPERTY(EditAnywhere, Category = "SpawnTimer")
	float SpawnTimer = 30.f;

	bool bCanSpawnBall;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Projectile")
		TArray<TSubclassOf<class ACPPBall>> Balls;

	ACPPBall* SpawnedBall;

	UFUNCTION(Server, Reliable, Category = Effects)
		void ServerPlayNiagara(UNiagaraComponent* fx, bool state);

	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = Effects)
		void MulticastPlayNiagara(UNiagaraComponent* fx, bool state);

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* ShowBallFX;

	//UPROPERTY(EditAnywhere, Category = "Gameplay|BallMesh")
	//	TArray<UStaticMesh*> Meshes;
		//UStaticMesh* Meshy;
	int32 RandIdx;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* SpawnVolume;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
