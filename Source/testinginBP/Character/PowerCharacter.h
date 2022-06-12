// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPTestCharacter.h"
#include "NiagaraComponent.h"
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


	UPROPERTY(Replicated)
		bool bSuperBall;

	FORCEINLINE bool IsSuperBall() const { return bSuperBall; }

	/*UPROPERTY()
		AGameHUD* GameHUD;*/

	UFUNCTION()
		void CreateHUD();

protected:
	virtual void BeginPlay() override;

	void AbilityCooldown();

	void DoAbility();

	void LoopHitActors();

	void StartAbilityTimer();

	void DoSweep();

	void SuperUpBall();

	UPROPERTY()
		float AbilityDelayTime = 1.f;

	void AbilityDelay();

	void AbilityDelayTimer();

	UFUNCTION(Server, Reliable)
	void Server_DoAbility();

	UPROPERTY(Replicated)
	bool bSmash;

	virtual void LockTarget() override;

	void LockTargetAbility();
	UPROPERTY(EditAnywhere, Category = "Ability")
		float Ability_Cooldown_Duration = 2.f;
	UPROPERTY(EditAnywhere, Category = "Ability")
		float StartDistance = 150.f;
	UPROPERTY(EditAnywhere, Category = "Ability")
		float EndDistance = 500.f;
	UPROPERTY(EditAnywhere, Category = "Ability")
		float RangeRadius = 100;

	TArray<FHitResult> OutHits;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AbilityAnim;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* PowerAbilityFX;

	void DisableEffect();

};
