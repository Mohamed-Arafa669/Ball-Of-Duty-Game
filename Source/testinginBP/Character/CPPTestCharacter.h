// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPPTestCharacter.generated.h"

UCLASS()
class TESTINGINBP_API ACPPTestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACPPTestCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //Replication
	virtual void PostInitializeComponents() override;
	void PlayThrowMontage();
	


protected:
	virtual void BeginPlay() override;

	void MoveForward(float value);
	void MoveRight(float value);
	void Turn(float value);
	void LookUp(float value);
	void EquipButtonPressed();
	//void Throw();
	void Catch();

	void ThrowButtonPressed();
	void ThrowButtonReleased();

private:
	UPROPERTY(visibleAnywhere, Category = Camera)
		class USpringArmComponent* cameraBoom;
	UPROPERTY(visibleAnywhere, Category = Camera)
		class UCameraComponent* followCamera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* overHeadWidget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* throwAnim;
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingBall) //Replication
		class ACPPBall* overlappingBall;

	UFUNCTION()
	void OnRep_OverlappingBall(ACPPBall* lastBall); //Replication

	UPROPERTY(visibleAnywhere)
		class UCombatComponent* combat;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	/*UFUNCTION(Server, Reliable)
		void ServerThrowButtonPressed();*/

public:	
	 void SetOverlappingBall(ACPPBall* cppBall);

	 bool IsBallEquipped();
};
