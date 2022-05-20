// Fill out your copyright notice in the Description page of Project Settings.


#include "StealCharacter.h"
#include "DrawDebugHelpers.h"
#include "testinginBP/GameComponents/CombatComponent.h"


AStealCharacter::AStealCharacter()
{
	/*CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	CableComponent->SetupAttachment(GetMesh())*/;

	bHook = false;

}

void AStealCharacter::BeginPlay()
{
	Super::BeginPlay();

}



void AStealCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStealCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void AStealCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &ThisClass::DoAbility);
}

void AStealCharacter::AbilityCooldown()
{
	bHook = false;
}

void AStealCharacter::TraceLine()
{
	GetController()->GetPlayerViewPoint(Loc, Rot);

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
		TraceLine();
		if (ACPPTestCharacter* Target = Cast<ACPPTestCharacter>(Hit.GetActor()))
		{
			UE_LOG(LogTemp, Warning, TEXT("hHhHAIHDOWA"));

			if (Target->IsBallEquipped())
			{
				UE_LOG(LogTemp, Warning, TEXT("dropyasta"));

				Target->combat->equippedBall->GetBallMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				Target->combat->equippedBall->SetBallState(EBallState::EBS_Dropped);
				//Target->combat->equippedBall->MoveHookedBall(this);
				Target->combat->equippedBall->GetBallMesh()->SetSimulatePhysics(true);
				combat->equippedBall = Target->combat->equippedBall;
				Target->combat->equippedBall = nullptr;
				combat->EquipBall(combat->equippedBall);
				combat->equippedBall->SetBallState(EBallState::EBS_Equipped);
			}
		}
	}
	else
		Server_DoAbility_Implementation();
}

void AStealCharacter::Server_DoAbility_Implementation()
{
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
		}
	}
}

