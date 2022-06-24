// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EasterTrigger.generated.h"

UCLASS()
class TESTINGINBP_API AEasterTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEasterTrigger();
	class UBoxComponent* AreaBox;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void OnSphereOverlap(
			UPrimitiveComponent* overlappedComponent,
			AActor* otherActor,
			UPrimitiveComponent* otherComponent,
			int32 otherBodyIndex,
			bool bFromSweep,
			const FHitResult& sweepResult
		);

	UFUNCTION()
		void OnSphereEndOverlap(
			UPrimitiveComponent* overlappedComponent,
			AActor* otherActor,
			UPrimitiveComponent* otherComponent,
			int32 otherBodyIndex
		);


	UFUNCTION(Server, Reliable)
		void Server_PlaySound();

	UPROPERTY(EditAnywhere)
		class USoundBase* SoundToPlay;

};
