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

	UPROPERTY(EditAnywhere, Category = "Gameplay|Projectile")
		TSubclassOf<class AHook>ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Projectile")
		TSubclassOf<class ACPPBall>BallClass;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* SpecialAbilityAnimation;

	AHook* SpawnHook;

protected:
	virtual void BeginPlay() override;
	virtual void LockTarget() override;

	void LockTargetAbility();
	void DoAbility();

	void StealBall(ACPPTestCharacter* Target);

	void ThrowTwice();

	UFUNCTION(Server, Reliable)
		void Server_DoAbility();

	UFUNCTION(Server, Reliable)
		void HandleFire();

	void AbilityCooldown();

	

	void TraceLine(); /// 
	//TODO remove UFUNCTION 


	UPROPERTY(Replicated)
		bool bHook;


	float CoolDownTime;
	
	FVector Loc;
	FRotator Rot;
	FHitResult Hit;
	FVector Start;
	FVector End;
	float HookDistance = 2000.0f;

	TArray<AActor*> OverlappingActors;
	TArray<ACPPTestCharacter*> OverlappingEnemies;

};


