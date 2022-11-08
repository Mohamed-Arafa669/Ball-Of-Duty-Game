// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPAnimInstance.h"
#include "CPPTestCharacter.h"
#include "Kismet/KismetMathLibrary.h"
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

	FVector Velocity = myCharacter->GetVelocity(); 
	Velocity.Z = 0.0f;
	speed = Velocity.Size();

	/*if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Emerald, FString::Printf(TEXT("front %f, sides %f"), Front, Sides));
	}*/

	Front = UKismetMathLibrary::Dot_VectorVector(Velocity, myCharacter->GetActorForwardVector());

	Sides = UKismetMathLibrary::Dot_VectorVector(Velocity, myCharacter->GetActorRightVector());
	 
	bIsInAir = myCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = myCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;

	//bBallEquipped = myCharacter->IsBallEquipped();

	bBallEquipped = myCharacter->IsBallEquipped();

	FRotator aimRotation = myCharacter->GetBaseAimRotation();
	//UE_LOG(LogTemp, Warning, TEXT("Aim Rotation Yaw %f: "), aimRotation.Yaw);
}
