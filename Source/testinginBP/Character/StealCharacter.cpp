// Fill out your copyright notice in the Description page of Project Settings.


#include "StealCharacter.h"
#include "DrawDebugHelpers.h"
#include "testinginBP/GameComponents/CombatComponent.h"
#include "testinginBP/HUD/GameHUD.h"
#include "testinginBP/Hook/Hook.h"
#include "Net/UnrealNetwork.h" //Replication


AStealCharacter::AStealCharacter()
{
	/*CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	CableComponent->SetupAttachment(GetMesh())*/;
	//ProjectileClass = AHook::StaticClass();

	bHook = false;

	CoolDownTime = 1.0f;

	cameraBoom->SetRelativeRotation(FRotator(0, 90, 0));
	cameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 300.f));
	cameraBoom->bUsePawnControlRotation = true;
}

void AStealCharacter::BeginPlay()
{
	Super::BeginPlay();
	bHook = true;
	CreateHUD();
}


void AStealCharacter::LockTargetAbility()
{
	if (bHook) {
		LockTarget();
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



void AStealCharacter::TraceLine()
{
	////TODO Parameters for Trace (location, Rotation)
	// //Local variables(not class)
	//TraceLine return Hit
	
		//GetController()->GetPlayerViewPoint(Loc, Rot);
	Rot = GetControlRotation();

	Start = GetActorLocation() + GetActorForwardVector() * 100.0f;
	//Start = GetActorLocation() + (GetControlRotation().Vector() * 100.0f) + (GetActorUpVector() * 100.0f);
	End = Start /*+ GetActorRotation().Vector()*/ + GetActorForwardVector() * HookDistance;  //(Rot.Vector() * HookDistance);

	FCollisionQueryParams TraceParams;

	UWorld* World = GetWorld();
	if (World)
	{
		bool b = World->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel2, TraceParams);
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
	else {
		Server_DoAbility();
	}

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
		//TraceLine();
		if (lockOnTargets->GetTarget()) {
			if (ACPPTestCharacter* Target = Cast<ACPPTestCharacter>(lockOnTargets->GetTarget()))
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
		}
		/*else
			CoolDownTime = 1.0f;*/
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::AbilityCooldown, CoolDownTime);
	}
	
}

void AStealCharacter::HandleFire_Implementation()
{
	//FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 500.0f) + (GetActorUpVector() * 100.0f);
	FVector spawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator spawnRotation = GetControlRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	SpawnHook = GetWorld()->SpawnActor<AHook>(ProjectileClass, spawnLocation, spawnRotation);	
}


void AStealCharacter::StealBall(ACPPTestCharacter* Target)
{

	//TODO General Unequip Function
	bSteal = true;
	SpawnHook->AttachToActor(Target->combat->equippedBall, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//Target->combat->equippedBall->GetAreaSphere()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	//Target->combat->equippedBall->SetBallState(EBallState::EBS_Initial);
	//Target->combat->equippedBall->MoveHookedBall(this);
	//combat->equippedBall = Target->combat->equippedBall;
	//Target->combat->equippedBall = nullptr;
	//Target->bEquipped = false;

	//combat->equippedBall->SetBallState(EBallState::EBS_Initial);

	ACPPBall* StolenBall = Target->combat->equippedBall;
	Target->combat->UnEquipBall(StolenBall);
	StolenBall->MoveHookedBall(this);
	SpawnHook->Destroy();
	
	ClearTarget();
}

void AStealCharacter::ThrowTwice()
{
	MyThrow();

	FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() ) + (GetActorUpVector() );
	FRotator spawnRotation = GetControlRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	ACPPBall* SpawnBall = GetWorld()->SpawnActor<ACPPBall>(BallClass, spawnLocation, spawnRotation);

	//FTimerHandle ClearHandle;
	//GetWorld()->GetTimerManager().SetTimer(ClearHandle, this, &ACPPTestCharacter::ClearTarget, 1.0f);
}


void AStealCharacter::CreateHUD()
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) : GameHUD;
	if (GameHUD)
	{
		GameHUD->AddMelodyProfiler();
		//GameHUD->AddCrimsonProfiler();

	}

}