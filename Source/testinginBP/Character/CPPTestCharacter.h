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

	void Dash();
	void CanDash();
	void ThrowButtonPressed();
	void ThrowButtonReleased();

private:
	UPROPERTY(visibleAnywhere, Category = Camera)
		class USpringArmComponent* cameraBoom;
	UPROPERTY(visibleAnywhere, Category = Camera)
		class UCameraComponent* followCamera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* overHeadWidget;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* throwAnim;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* DashAnim;
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingBall) //Replication
		class ACPPBall* overlappingBall;

	UFUNCTION()
	void OnRep_OverlappingBall(ACPPBall* lastBall); //Replication

	UPROPERTY(visibleAnywhere)
		class UCombatComponent* combat;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	UFUNCTION(Server, Reliable)
		void ServerThrowButtonPressed();

	UFUNCTION(Server, Reliable)
	void DashButtonPressed();

	UFUNCTION(Server, Reliable, WithValidation, Category = Animation)
		void ServerPlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = Animation)
		void MulticastPlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);
	/*UFUNCTION(Server, Reliable)
		void ServerThrowButtonPressed();*/

public:	
	 void SetOverlappingBall(ACPPBall* cppBall);

	 bool IsBallEquipped();

	 bool bCanDash;

	UPROPERTY(EditAnywhere, Replicated, Category = "Movement")
	 float DashDistance = 6000.f;



};
