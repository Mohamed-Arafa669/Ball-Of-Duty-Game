// Fill out your copyright notice in the Description page of Project Settings.

#include "PowerCharacter.h"
#include <string>
#include "Components/SceneComponent.h"
#include "testinginBP/HUD/GameHUD.h"
#include "Net/UnrealNetwork.h"
#include "CPPTestCharacter.h"
#include "testinginBP/GameComponents/CombatComponent.h"


APowerCharacter::APowerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	PowerAbilityFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PowerAbilityFX"));
	PowerAbilityFX->SetupAttachment(GetMesh());

	cameraBoom->SetRelativeRotation(FRotator(0, 90, 0));
	cameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	cameraBoom->bUsePawnControlRotation = true;
}

void APowerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void APowerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &ThisClass::LockTargetAbility);
	PlayerInputComponent->BindAction("Ability", IE_Released, this, &ThisClass::DoAbility);

}

void APowerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APowerCharacter, bSmash);
	DOREPLIFETIME(APowerCharacter, bSuperBall);
	DOREPLIFETIME(APowerCharacter, bDoingAbility);
	DOREPLIFETIME(APowerCharacter, AbilityTime);

}

void APowerCharacter::IncreaseAbilityCharge()
{
	if (GetLocalRole() == ROLE_Authority) {
		if (AbilityTime <= (Ability_Cooldown_Duration) / (Ability_Cooldown_Duration))
		{
			AbilityTime += (0.1f / (float)Ability_Cooldown_Duration);
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(AbilityHandler);
		}
	}
}

void APowerCharacter::BeginPlay()
{
	Super::BeginPlay();

	bSmash = false;

	CreateHUD();

	AbilityTime = 0;
	GetWorld()->GetTimerManager().SetTimer(AbilityHandler, this, &ThisClass::IncreaseAbilityCharge, 0.1f, true);
	StartAbilityTimer();

}

void APowerCharacter::AbilityCooldown()
{
	bSmash = true;
	OutHits.Empty();
}

void APowerCharacter::LoopHitActors()
{
	if (OutHits.Num() > 0) {
		for (int i = 0; i < OutHits.Num(); i++)
		{
			if (ACPPTestCharacter* Temp = Cast<ACPPTestCharacter>(OutHits[i].GetActor())) {

				if (Temp == this) continue;
				Temp->Stunned();
			}
		}
	}
}

void APowerCharacter::StartAbilityTimer()
{
	AbilityTime = 0;
	FTimerHandle CooldownHandler;
	GetWorld()->GetTimerManager().SetTimer(CooldownHandler, this, &ThisClass::AbilityCooldown, Ability_Cooldown_Duration);
}

void APowerCharacter::DoSweep()
{
	FVector Start = GetActorLocation() + GetActorForwardVector() * StartDistance;
	FVector End = Start + (GetControlRotation().Vector() * EndDistance);

	GetWorld()->SweepMultiByChannel(OutHits, Start, End, FQuat::Identity, ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(RangeRadius));

	LoopHitActors();

	if (PowerAbilityFX) {
		MulticastPlayNiagara(PowerAbilityFX, true);
		ServerPlayNiagara(PowerAbilityFX, true);

		FTimerHandle AbilityTimer;
		GetWorld()->GetTimerManager().SetTimer(AbilityTimer, this, &APowerCharacter::DisableEffect, 0.7f);
	}
	ServerPlaySounds(AbilityWithoutBall, GetActorLocation());

	LoopHitActors();

	ClearTarget();

}

void APowerCharacter::SuperUpBall()
{
	ServerPlaySounds(AbilityWithBall, GetActorLocation());
	combat->equippedBall->SetBallState(EBallState::EBS_SuperThrow);
	MyThrow();

	ClearTarget();

}

void APowerCharacter::AbilityDelay()
{

	GetWorld()->GetTimerManager().SetTimer(AbilityHandler, this, &ThisClass::IncreaseAbilityCharge, 0.1f, true);

	if (!IsBallEquipped())
	{
		DoSweep();
	}
	else if (IsBallEquipped())
	{
		SuperUpBall();
	}

	ClearTarget();
}

void APowerCharacter::AbilityDelayTimer()
{
	FTimerHandle AbilityDelay;
	GetWorld()->GetTimerManager().SetTimer(AbilityDelay, this, &ThisClass::AbilityDelay, AbilityDelayTime);
}

void APowerCharacter::LockTarget()
{
	Super::LockTarget();
}

void APowerCharacter::LockTargetAbility()
{
	if (bSmash) {
		if (HasAuthority())
		{
			LockTarget();
			bDoingAbility = true;
			if (IsBallEquipped()) {

				combat->equippedBall->SetBallState(EBallState::EBS_SuperThrow);
			}

		}
		else
		{
			LockTarget();
			bDoingAbility = true;
			if (IsBallEquipped()) {

				combat->equippedBall->SetBallState(EBallState::EBS_SuperThrow);
			}
		}

	}
}

void APowerCharacter::DisableEffect()
{
	if (HasAuthority())
	{
		MulticastPlayNiagara(PowerAbilityFX, false);
		ServerPlayNiagara(PowerAbilityFX, false);

	}
	else
	{
		MulticastPlayNiagara(PowerAbilityFX, false);
		ServerPlayNiagara(PowerAbilityFX, false);

	}
	ClearTarget();
}

void APowerCharacter::DoAbility()
{

	if (bSmash) {

		if (HasAuthority()) {
			bDoingAbility = false;
			bSmash = false;
			StartAbilityTimer();

			if (AbilityAnim)
			{
				MulticastPlayAnimMontage(AbilityAnim, 1, NAME_None);
			}

			AbilityDelayTimer();
		}
		else
		{
			bDoingAbility = false;
			bSmash = false;
			Server_DoAbility();
		}
	}
}

void APowerCharacter::Server_DoAbility_Implementation()
{
	bDoingAbility = false;
	bSmash = false;
	StartAbilityTimer();

	if (AbilityAnim)
	{
		PlayAnimMontage(AbilityAnim, 1, NAME_None);
		ServerPlayAnimMontage(AbilityAnim, 1, NAME_None);
	}

	AbilityDelayTimer();

}

void APowerCharacter::CreateHUD()
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) : GameHUD;
	
}

