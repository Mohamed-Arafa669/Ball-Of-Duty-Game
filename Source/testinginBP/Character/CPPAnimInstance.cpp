// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPAnimInstance.h"
#include "CPPTestCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCPPAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	myCharacter = Cast<ACPPTestCharacter>(TryGetPawnOwner());
}

void UCPPAnimInstance::NativeUpdateAnimation(float deltaTime)
{
	Super::NativeUpdateAnimation(deltaTime);

	if (myCharacter == nullptr)
	{
		myCharacter = Cast<ACPPTestCharacter>(TryGetPawnOwner());
	}
	if (myCharacter == nullptr) return;

	FVector velocity = myCharacter->GetVelocity(); 
	velocity.Z = 0.0f;
	speed = velocity.Size();
	 
	bIsInAir = myCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = myCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;

	//bBallEquipped = myCharacter->IsBallEquipped();

	bBallEquipped = myCharacter->bEquipped;

	FRotator aimRotation = myCharacter->GetBaseAimRotation();
	//UE_LOG(LogTemp, Warning, TEXT("Aim Rotation Yaw %f: "), aimRotation.Yaw);
}
