// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPTestCharacter.h"
#include "StealCharacter.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API AStealCharacter : public ACPPTestCharacter
{
	GENERATED_BODY()

public:
	AStealCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	virtual void BeginPlay() override;

	void DoAbility();

	UFUNCTION(Server, Reliable)
		void Server_DoAbility();


	void AbilityCooldown();

	void TraceLine();

	UPROPERTY(Replicated)
		bool bHook;


	FVector Loc;
	FRotator Rot;
	FHitResult Hit;
	FVector Start;
	FVector End;
	float HookDistance = 2000.0f;

	TArray<AActor*> OverlappingActors;
	TArray<ACPPTestCharacter*> OverlappingEnemies;
};
