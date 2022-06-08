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
	

protected:
	virtual void BeginPlay() override;


	void AbilityCooldown();

	void DoAbility();

	UFUNCTION(Server, Reliable)
	void Server_DoAbility();

	UPROPERTY(Replicated)
	bool bSmash;

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
};
