// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPTestCharacter.h"
#include "Net/UnrealNetwork.h"
#include "PowerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API APowerCharacter : public ACPPTestCharacter
{
	GENERATED_BODY()

public:
	APowerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* HitAreaCone;

protected:
	virtual void BeginPlay() override;


	void AbilityCooldown();
	void EffectCooldown();

	
	void DoAbility();

	UFUNCTION(Server, Reliable)
	void Server_DoAbility();

	UPROPERTY(Replicated)
	bool bSmash;

	bool bEffectOn;

	TArray<AActor*> OverlappingActors;
	TArray<ACPPTestCharacter*> OverlappingEnemies;
};
