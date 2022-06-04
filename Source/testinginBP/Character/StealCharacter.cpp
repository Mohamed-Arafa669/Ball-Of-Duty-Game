// Fill out your copyright notice in the Description page of Project Settings.


#include "StealCharacter.h"
#include "DrawDebugHelpers.h"
#include "testinginBP/GameComponents/CombatComponent.h"
#include "testinginBP/Hook/Hook.h"
#include "Net/UnrealNetwork.h" //Replication


AStealCharacter::AStealCharacter()
{
	/*CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	CableComponent->SetupAttachment(GetMesh())*/;
	//ProjectileClass = AHook::StaticClass();

	bHook = false;

	CoolDownTime = 1.0f;

}

void AStealCharacter::BeginPlay()
{
	Super::BeginPlay();
	bHook = true;
}



void AStealCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStealCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AStealCharacter, bHook);

}

void AStealCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &ThisClass::DoAbility);
}



void AStealCharacter::AbilityCooldown()
{
	bHook = true;
}



void AStealCharacter::TraceLine()
{
	////TODO Parameters for Trace (location, Rotation)
	// //Local variables(not class)
	//TraceLine return Hit
	
		//GetController()->GetPlayerViewPoint(Loc, Rot);
	Rot = GetControlRotation();

		Start = GetActorLocation() + GetActorForwardVector() * 100.0f;
		End = Start + GetActorRotation().Vector() /*+ GetActorForwardVector()*/ * HookDistance;  //(Rot.Vector() * HookDistance);

		FCollisionQueryParams TraceParams;

		UWorld* World = GetWorld();
		if (World)
		{
			bool b = World->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel1, TraceParams);
			if (b)
				DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5.0f, 5.0f, 5.0f), FColor::Yellow, false, 2.0f);
		}
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 2.0f);

	

	//UE_LOG(LogTemp, Warning,TEXT("HhHH %s"), *Hit.GetActor()->GetFName().ToString());
}

void AStealCharacter::DoAbility()
{
	if (HasAuthority())
	{
		if (bHook) {

			if (SpecialAbilityAnimation)
			{
				MulticastPlayAnimMontage(SpecialAbilityAnimation);
			}
			bHook = false; //hook cool down (bCanHook)
			HandleFire();
			TraceLine();
			if (ACPPTestCharacter* Target = Cast<ACPPTestCharacter>(Hit.GetActor()))
			{
				UE_LOG(LogTemp, Warning, TEXT("hHhHAIHDOWA"));
				CoolDownTime = 10.0f;
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
				CoolDownTime = 1.0f;

			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::AbilityCooldown, CoolDownTime);
		}
	}
	else
		Server_DoAbility();
}



void AStealCharacter::Server_DoAbility_Implementation()
{
	if (bHook) {
		bHook = false;

		if (SpecialAbilityAnimation)
		{
			PlayAnimMontage(SpecialAbilityAnimation);
			ServerPlayAnimMontage(SpecialAbilityAnimation);
		}
		HandleFire();
		TraceLine();
		if (ACPPTestCharacter* Target = Cast<ACPPTestCharacter>(Hit.GetActor()))
		{
			UE_LOG(LogTemp, Warning, TEXT("hHhHAIHDOWA"));
			CoolDownTime = 10.0f;

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
			CoolDownTime = 1.0f;
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::AbilityCooldown, CoolDownTime);
	}
}

void AStealCharacter::HandleFire_Implementation()
{
	FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 500.0f) + (GetActorUpVector() * 100.0f);
	FRotator spawnRotation = GetControlRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	AHook* SpawnHook = GetWorld()->SpawnActor<AHook>(ProjectileClass, spawnLocation, spawnRotation);	

}


void AStealCharacter::StealBall(ACPPTestCharacter* Target)
{

	//TODO General Unequip Function
	bSteal = true;
	Target->combat->equippedBall->GetBallMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Target->combat->equippedBall->SetBallState(EBallState::EBS_Dropped);
	Target->combat->equippedBall->MoveHookedBall(this);
	combat->equippedBall = Target->combat->equippedBall;
	Target->combat->equippedBall = nullptr;
	Target->bEquipped = false;
}

void AStealCharacter::ThrowTwice()
{
	MyThrow();

	FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() ) + (GetActorUpVector() );
	FRotator spawnRotation = GetControlRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	ACPPBall* SpawnHook = GetWorld()->SpawnActor<ACPPBall>(BallClass, spawnLocation, spawnRotation);
}

