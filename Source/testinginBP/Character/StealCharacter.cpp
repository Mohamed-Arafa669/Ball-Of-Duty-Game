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

void AStealCharacter::TraceLine_Implementation()
{
	
		//GetController()->GetPlayerViewPoint(Loc, Rot);
	Rot = GetControlRotation();

		Start = GetActorLocation() + GetActorForwardVector() * 100.0f;
		End = Start + (Rot.Vector() * HookDistance);

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
			bHook = false;
			HandleFire();
			TraceLine();
			if (ACPPTestCharacter* Target = Cast<ACPPTestCharacter>(Hit.GetActor()))
			{
				UE_LOG(LogTemp, Warning, TEXT("hHhHAIHDOWA"));

				if (Target->IsBallEquipped())
				{
					UE_LOG(LogTemp, Warning, TEXT("dropyasta"));

					Target->combat->equippedBall->GetBallMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
					Target->combat->equippedBall->SetBallState(EBallState::EBS_Dropped);
					Target->combat->equippedBall->MoveHookedBall(this);
					//Target->combat->equippedBall->GetBallMesh()->SetSimulatePhysics(true);
					combat->equippedBall = Target->combat->equippedBall;
					Target->combat->equippedBall = nullptr;
					combat->EquipBall(combat->equippedBall);
					combat->equippedBall->SetBallState(EBallState::EBS_Equipped);

				}
			}
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::AbilityCooldown, 1.f);
		}
	}
	else
		Server_DoAbility();
}

void AStealCharacter::Server_DoAbility_Implementation()
{
	if (bHook) {
		bHook = false;
		HandleFire();
		TraceLine();
		if (ACPPTestCharacter* Target = Cast<ACPPTestCharacter>(Hit.GetActor()))
		{
			UE_LOG(LogTemp, Warning, TEXT("hHhHAIHDOWA"));

			if (Target->IsBallEquipped())
			{
				UE_LOG(LogTemp, Warning, TEXT("dropyasta"));
				

				Target->combat->equippedBall->GetBallMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				Target->combat->equippedBall->SetBallState(EBallState::EBS_Dropped);
				Target->combat->equippedBall->MoveHookedBall(this);
				//Target->combat->equippedBall->GetBallMesh()->SetSimulatePhysics(true);
				combat->equippedBall = Target->combat->equippedBall;
				Target->combat->equippedBall = nullptr;
				Target->bEquipped = false;
				combat->EquipBall(combat->equippedBall);

				
			}
		}
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::AbilityCooldown, 1.f);
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