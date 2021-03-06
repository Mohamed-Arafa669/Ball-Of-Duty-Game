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


	UFUNCTION(BlueprintPure, Category = "AbilityCoolDown")
		FORCEINLINE float GetAbilityTime() const { return AbilityTime; }

	UPROPERTY(Replicated)
	bool isBungeeGum;


	void HomeOnMe();

	UFUNCTION(Server, Reliable)
	void ServerHomeOnMe();

	UPROPERTY(Replicated)
		ACPPBall* BungeeBall;


	UPROPERTY(EditAnywhere, Category = "Gameplay|Projectile")
		TSubclassOf<class AHook>ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Gameplay|Projectile")
		TSubclassOf<class ACPPBall>BallClass;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* SpecialAbilityAnimation;

	AHook* SpawnHook;

	UFUNCTION()
		void CreateHUD();
	void ThrowTwice();

protected:
	virtual void BeginPlay() override;

	void LockTargetAbility();
	void DoAbility();

	void StealBall(ACPPTestCharacter* Target);


	UFUNCTION(Server, Reliable)
		void Server_DoAbility();

	void AbilityCooldown();

	void DestroyHook();

	UPROPERTY(Replicated)
		bool bHook;

	UPROPERTY(EditAnywhere, Replicated)
		float AbilityTime;

	void IncreaseAbilityCharge();

	FTimerHandle AbilityHandler;


	
	UPROPERTY(EditAnywhere, Category= "Abilities")
	float CoolDownTime;
	
	FVector Loc;
	FRotator Rot;
	FHitResult Hit;
	FVector Start;
	FVector End;
	float HookDistance = 2000.0f;

};


