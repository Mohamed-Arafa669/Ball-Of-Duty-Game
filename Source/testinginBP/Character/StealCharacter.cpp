// Fill out your copyright notice in the Description page of Project Settings.


#include "StealCharacter.h"
#include "DrawDebugHelpers.h"
#include "testinginBP/GameComponents/CombatComponent.h"
#include "testinginBP/HUD/GameHUD.h"
#include "testinginBP/Hook/Hook.h"
#include "Net/UnrealNetwork.h" //Replication


AStealCharacter::AStealCharacter()
{
	cameraBoom->SetRelativeRotation(FRotator(0, 90, 0));
	cameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 300.f));
	cameraBoom->bUsePawnControlRotation = true;

	bHook = false;
	CoolDownTime = 1.0f;

}

void AStealCharacter::BeginPlay()
{
	Super::BeginPlay();
	bHook = false;
	isBungeeGum = false;
	CreateHUD();

	AbilityTime = 0;
	GetWorld()->GetTimerManager().SetTimer(AbilityHandler, this, &ThisClass::IncreaseAbilityCharge, 0.1f, true);

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::AbilityCooldown, CoolDownTime);
}

void AStealCharacter::LockTargetAbility()
{
	if (bHook && !bKnocked && !bStunned) {
		if (HasAuthority())
		{
			LockTarget();
			bDoingAbility = true;

		}
		else
		{
			LockTarget();
			bDoingAbility = true;

		}
	}
}

void AStealCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStealCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AStealCharacter, bHook);
	DOREPLIFETIME(AStealCharacter, isBungeeGum);
	DOREPLIFETIME(AStealCharacter, BungeeBall);
	DOREPLIFETIME(AStealCharacter, bDoingAbility);
	DOREPLIFETIME(AStealCharacter, AbilityTime);

}

void AStealCharacter::HomeOnMe()
{
	if (HasAuthority())
	{
		BungeeBall->SetBallState(EBallState::EBS_Stolen);
		BungeeBall->ProjectileMovementComponent->HomingTargetComponent = GetRootComponent();

	}
	else
	{
		ServerHomeOnMe();
	}
}

void AStealCharacter::ServerHomeOnMe_Implementation()
{
	BungeeBall->SetBallState(EBallState::EBS_Stolen);
	BungeeBall->ProjectileMovementComponent->HomingTargetComponent = GetRootComponent();

}

void AStealCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &ThisClass::LockTargetAbility);
	PlayerInputComponent->BindAction("Ability", IE_Released, this, &ThisClass::DoAbility);

}

void AStealCharacter::AbilityCooldown()
{
	bHook = true;
}


void AStealCharacter::IncreaseAbilityCharge()
{
	if (GetLocalRole() == ROLE_Authority) {
		if (AbilityTime <= (CoolDownTime) / (CoolDownTime))
		{
			AbilityTime += (0.1f / (float)CoolDownTime);
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(AbilityHandler);
		}
	}
}

void AStealCharacter::DoAbility()
{
	if (bHook && !bKnocked && !bStunned)
	{
		if (HasAuthority()) {

			if (ACPPTestCharacter* Target = Cast<ACPPTestCharacter>(lockOnTargets->GetTarget()))
			{

				bDoingAbility = false;
				bHook = false; //hook cool down (bCanHook)
				if (SpecialAbilityAnimation)
				{
					MulticastPlayAnimMontage(SpecialAbilityAnimation);
				}

				GetWorld()->GetTimerManager().SetTimer(AbilityHandler, this, &ThisClass::IncreaseAbilityCharge, 0.1f, true);
				AbilityTime = 0;

				if (Target->IsBallEquipped() && !IsBallEquipped())
				{
					StealBall(Target);
				}
				else if (IsBallEquipped())
				{
					ThrowTwice();
				}

			}
			else
			{
				bDoingAbility = false;
			}

			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::AbilityCooldown, CoolDownTime);
		}
		else {
			bDoingAbility = false;
			Server_DoAbility();
		}

		ClearTarget();
	}
}

void AStealCharacter::Server_DoAbility_Implementation()
{
	if (lockOnTargets->GetTarget()) {

		if (ACPPTestCharacter* Target = Cast<ACPPTestCharacter>(lockOnTargets->GetTarget()))
		{
			bHook = false;
			GetWorld()->GetTimerManager().SetTimer(AbilityHandler, this, &ThisClass::IncreaseAbilityCharge, 0.1f, true);
			AbilityTime = 0;

			if (SpecialAbilityAnimation)
			{
				PlayAnimMontage(SpecialAbilityAnimation);
				ServerPlayAnimMontage(SpecialAbilityAnimation);
			}

			if (Target->IsBallEquipped() && !IsBallEquipped())
			{
				StealBall(Target);
			}
			else if (IsBallEquipped())
			{
				ThrowTwice();

			}
		}
	}
	else
	{
		bDoingAbility = false;
	}
	
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::AbilityCooldown, CoolDownTime);

}

void AStealCharacter::StealBall(ACPPTestCharacter* Target)
{
	
	ServerPlaySounds(AbilityWithoutBall, GetActorLocation());
	
	bSteal = true;
	SpawnHook = GetWorld()->SpawnActor<AHook>(ProjectileClass, Target->GetActorLocation(), Target->GetActorRotation());


	ACPPBall* StolenBall = Target->combat->equippedBall;
	Target->combat->UnEquipBall(StolenBall);


	StolenBall->SetBallState(EBallState::EBS_Stolen);
	StolenBall->ProjectileMovementComponent->HomingTargetComponent = GetRootComponent();

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::DestroyHook, 5.0f);

}

void AStealCharacter::DestroyHook()
{
	if (SpawnHook)
		SpawnHook->Destroy();
}

void AStealCharacter::ThrowTwice()
{
	
	ServerPlaySounds(AbilityWithBall, GetActorLocation());
	BungeeBall = nullptr;
	BungeeBall = combat->equippedBall;
	BungeeBall->SetBallState(EBallState::EBS_Stolen);
	isBungeeGum = true;
	MyThrow();


}

void AStealCharacter::CreateHUD()
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) : GameHUD;

}