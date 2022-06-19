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
	if (bHook) {
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
		/*FString msg1 = OtherActor->GetName();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, *msg1);*/
		BungeeBall->SetBallState(EBallState::EBS_Stolen);
		BungeeBall->ProjectileMovementComponent->HomingTargetComponent = GetRootComponent();
		
	} else
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
			//AbilityTime = Ability_Cooldown_Duration;
		}
	}
}

void AStealCharacter::DoAbility()
{

	if (bHook)
	{
		if (HasAuthority()) {
			//HandleFire();
			//TraceLine();
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
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), BungeeGum, GetActorLocation());

				//CoolDownTime = 1.0f;
				if (Target->IsBallEquipped() && !IsBallEquipped())
				{
					StealBall(Target);
					
					//HandleFire();
				}
				else if (IsBallEquipped())
				{
					ThrowTwice();
					
				}

			}
			/*else
				CoolDownTime = 1.0f;*/

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
		
		if (SpecialAbilityAnimation)
		{
			PlayAnimMontage(SpecialAbilityAnimation);
			ServerPlayAnimMontage(SpecialAbilityAnimation);
		}
		//HandleFire();

		//TraceLine();
		if (lockOnTargets->GetTarget()) {
			if (ACPPTestCharacter* Target = Cast<ACPPTestCharacter>(lockOnTargets->GetTarget()))
			{
				bHook = false;
				GetWorld()->GetTimerManager().SetTimer(AbilityHandler, this, &ThisClass::IncreaseAbilityCharge, 0.1f, true);
				AbilityTime = 0;
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), BungeeGum, GetActorLocation());


				if (Target->IsBallEquipped() && !IsBallEquipped())
				{
					StealBall(Target);
					
					//HandleFire();
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

//void AStealCharacter::HandleFire_Implementation()
//{
//	////FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 500.0f) + (GetActorUpVector() * 100.0f);
//	FVector spawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
//	FRotator spawnRotation = GetControlRotation();
//
//	FActorSpawnParameters spawnParameters;
//	spawnParameters.Instigator = GetInstigator();
//	spawnParameters.Owner = this;
//
//	SpawnHook = GetWorld()->SpawnActor<AHook>(ProjectileClass, spawnLocation, spawnRotation);
//
//		//TODO General Unequip Function
//	//bSteal = true;
//	//SpawnHook->AttachToActor(Target->combat->equippedBall, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
//	//Target->combat->equippedBall->SetBallState(EBallState::EBS_Initial);
//	//Target->combat->equippedBall->MoveHookedBall(this);
//	//combat->equippedBall = Target->combat->equippedBall;
//
//	/*Target->combat->equippedBall->GetAreaSphere()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
//	Target->combat->equippedBall = nullptr;
//	Target->bEquipped = false;*/
//	
//}


void AStealCharacter::StealBall(ACPPTestCharacter* Target)
{

	//TODO General Unequip Function
	bSteal = true;
	SpawnHook = GetWorld()->SpawnActor<AHook>(ProjectileClass, Target->GetActorLocation(), Target->GetActorRotation());

	//SpawnHook->AttachToActor(Target->combat->equippedBall, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//Target->combat->equippedBall->SetBallState(EBallState::EBS_Initial);
	//Target->combat->equippedBall->MoveHookedBall(this);
	//combat->equippedBall = Target->combat->equippedBall;

	/*Target->combat->equippedBall->GetAreaSphere()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Target->combat->equippedBall = nullptr;
	Target->bEquipped = false;*/

	
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
	BungeeBall = nullptr;
	BungeeBall = combat->equippedBall;
	BungeeBall->SetBallState(EBallState::EBS_Stolen);
	isBungeeGum = true;
	MyThrow();

	
	/*FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() ) + (GetActorUpVector() );
	FRotator spawnRotation = GetControlRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	ACPPBall* SpawnBall = GetWorld()->SpawnActor<ACPPBall>(BallClass, spawnLocation, spawnRotation);

	combat->EquipBall(SpawnBall);*/

	//FTimerHandle ClearHandle;
	//GetWorld()->GetTimerManager().SetTimer(ClearHandle, this, &ACPPTestCharacter::ClearTarget, 1.0f);
}


void AStealCharacter::CreateHUD()
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) : GameHUD;
	if (GameHUD)
	{
		//GameHUD->AddMelodyProfiler();
		//GameHUD->AddCrimsonProfiler();

	}

}