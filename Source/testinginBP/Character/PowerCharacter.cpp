// Fill out your copyright notice in the Description page of Project Settings.

#include "PowerCharacter.h"

#include <string>

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
	DOREPLIFETIME(APowerCharacter, bDoingAbility);
	DOREPLIFETIME(APowerCharacter, AbilityTime);


	

}

void APowerCharacter::IncreaseAbilityCharge()
{
	if (GetLocalRole() == ROLE_Authority) {
		if (AbilityTime <= (Ability_Cooldown_Duration) / (Ability_Cooldown_Duration))
		{

			AbilityTime += (0.1f / (float)Ability_Cooldown_Duration);
			/*FString msg = FString::Printf(TEXT("%f"), AbilityTime);
			GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Blue, msg);*/

		}
		else
		{
			/*FString msg1 = TEXT("?????????????????");
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, msg1);*/
			GetWorld()->GetTimerManager().ClearTimer(AbilityHandler);
			//AbilityTime = Ability_Cooldown_Duration;
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

			//DrawDebugSphere(GetWorld(), OutHits[i].GetActor()->GetActorLocation(), 100.f, 30, FColor::Red, false, 2.f);

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
		//UGameplayStatics::PlaySoundAtLocation(GetWorld(), AbilityWithoutBall, GetActorLocation());
		ServerPlaySounds(AbilityWithoutBall, GetActorLocation());

		LoopHitActors();

}

void APowerCharacter::SuperUpBall()
{

	//UGameplayStatics::PlaySoundAtLocation(GetWorld(), AbilityWithBall, GetActorLocation());
	ServerPlaySounds(AbilityWithBall, GetActorLocation());
	combat->equippedBall->SetBallState(EBallState::EBS_SuperThrow);
	MyThrow();

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

		} else
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
	if(HasAuthority())
	{
		MulticastPlayNiagara(PowerAbilityFX, false);
		ServerPlayNiagara(PowerAbilityFX, false);
		
	} else
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
	//bDoingAbility = false;
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
		///
		// GameHUD->AddCrimsonProfiler();
		//GameHUD->AddMelodyProfiler();

	}
}

