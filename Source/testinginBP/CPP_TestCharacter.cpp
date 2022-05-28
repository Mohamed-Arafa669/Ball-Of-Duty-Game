// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_TestCharacter.h"

// Sets default values
ACPP_TestCharacter::ACPP_TestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACPP_TestCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACPP_TestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACPP_TestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACPP_TestCharacter::MoveForward(float AxisVal)
{

}

void ACPP_TestCharacter::MoveRight(float AxisVal)
{

}

