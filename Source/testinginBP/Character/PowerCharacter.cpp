// Fill out your copyright notice in the Description page of Project Settings.

#include "PowerCharacter.h"
#include "Components/SceneComponent.h"
#include "testinginBP/HUD/GameHUD.h"
#include "Net/UnrealNetwork.h"
#include "CPPTestCharacter.h"
#include "DrawDebugHelpers.h"
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

}

void APowerCharacter::BeginPlay()
{
	Super::BeginPlay();

	bSmash = false;

	CreateHUD();

}

void APowerCharacter::AbilityCooldown()
{
	bSmash = false;
	OutHits.Empty();
}

void APowerCharacter::LoopHitActors()
{
	if (OutHits.Num() > 0) {
		for (int i = 0; i < OutHits.Num(); i++)
		{

			DrawDebugSphere(GetWorld(), OutHits[i].GetActor()->GetActorLocation(), 100.f, 30, FColor::Red, false, 2.f);

			if (ACPPTestCharacter* Temp = Cast<ACPPTestCharacter>(OutHits[i].GetActor())) {


				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1,
						5.f, FColor::Green, FString(Temp->GetName()));
				}
				if (Temp == this) continue;
				Temp->Stunned();
			}
		}
	}
}

void APowerCharacter::StartAbilityTimer()
{
	bSmash = true;
	FTimerHandle AbilityHandle;
	GetWorld()->GetTimerManager().SetTimer(AbilityHandle, this, &ThisClass::AbilityCooldown, Ability_Cooldown_Duration);
}

void APowerCharacter::DoSweep()
{
	if (!IsBallEquipped()) {

		FVector Start = GetActorLocation() + GetActorForwardVector() * StartDistance;
		FVector End = Start + (GetControlRotation().Vector() * EndDistance);

		GetWorld()->SweepMultiByChannel(OutHits, Start, End, FQuat::Identity, ECollisionChannel::ECC_Pawn,
			FCollisionShape::MakeSphere(RangeRadius));

		LoopHitActors();

		if (PowerAbilityFX) {
			MulticastPlayNiagara(PowerAbilityFX, true);
			ServerPlayNiagara(PowerAbilityFX, true);

			FTimerHandle AbilityTimer;
			GetWorld()->GetTimerManager().SetTimer(AbilityTimer, this, &APowerCharacter::DisableEffect, 0.5f);
		}
	}
}

void APowerCharacter::SuperUpBall()
{
	if (IsBallEquipped())
	{
		combat->equippedBall->SetBallState(EBallState::EBS_SuperThrow);
		MyThrow();
	}
}

void APowerCharacter::AbilityDelay()
{
	DoSweep();
	SuperUpBall();
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
	if (!bSmash) {

		LockTarget();

		if (IsBallEquipped()) {
			combat->equippedBall->SetBallState(EBallState::EBS_SuperThrow);
		}

	}
}

void APowerCharacter::DisableEffect()
{
	MulticastPlayNiagara(PowerAbilityFX, false);
	ServerPlayNiagara(PowerAbilityFX, false);
}

void APowerCharacter::DoAbility()
{

	if (!bSmash) {

		FTimerHandle ClearHandle;
		GetWorld()->GetTimerManager().SetTimer(ClearHandle, this, &ACPPTestCharacter::ClearTarget, 1.1f);
		
		if (HasAuthority()) {

			StartAbilityTimer();

			

			if (AbilityAnim)
			{
				MulticastPlayAnimMontage(AbilityAnim, 1, NAME_None);
			}

			AbilityDelayTimer();
		}
		else
		{
			Server_DoAbility();
		}
	}
}

void APowerCharacter::Server_DoAbility_Implementation()
{
	StartAbilityTimer();

	/*FTimerHandle ClearHandle1;
	GetWorld()->GetTimerManager().SetTimer(ClearHandle1, this, &ACPPTestCharacter::ClearTarget, 1.1f);*/

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
	if (GameHUD)
	{
		//GameHUD->AddCrimsonProfiler();
		GameHUD->AddMelodyProfiler();

	}
}

