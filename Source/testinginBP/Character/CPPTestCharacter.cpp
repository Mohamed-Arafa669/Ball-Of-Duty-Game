

#include "CPPTestCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h" //Replication
#include "testinginBP\Ball\CPPBall.h"
#include "testinginBP\GameComponents\CombatComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "testinginBP\Character\CPPAnimInstance.h"
#include "Kismet//KismetMathLibrary.h"
#include "LockOnTargetComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "LockOnSubobjects/RotationModes/RotationModeBase.h"
#include "LockOnSubobjects/TargetHandlers/TargetHandlerBase.h"
#include "TargetingHelperComponent.h"
#include "../../LockOnTarget/Source/LockOnTarget/Public/LockOnTargetComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "testinginBP/GameMode/MyCPPGameModeBase.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "testinginBP/PlayerController/CPPPlayerController.h"

#include "testinginBP/PlayerController/CPPPlayerController.h"
#include "testinginBP/GameComponents/MyPlayerState.h"
#include "testinginBP/GameMode/MyGameMode.h"

#include "TimerManager.h"

ACPPTestCharacter::ACPPTestCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	cameraBoom->SetupAttachment(RootComponent);
	cameraBoom->TargetArmLength = 300.0f;


	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);
	

	overHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	overHeadWidget->SetupAttachment(RootComponent);

	lockOnTargets = CreateDefaultSubobject<ULockOnTargetComponent>(TEXT("LockOnTarget"));
	lockOnTargets->SetIsReplicated(true);

	targetComponent = CreateDefaultSubobject<UTargetingHelperComponent>(TEXT("targetComponent"));
	targetComponent->SetIsReplicated(true);

	combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	combat->SetIsReplicated(true);

	if (!LockOnTargetComponent)
	{
		LockOnTargetComponent = CreateDefaultSubobject<ULockOnTargetComponent>(TEXT("LockOnTargetComponent"));
	}

	CharacterMesh = GetMesh();

	bCanDash = true;

	bThrown = false;

	bCanThrow = false;

	bEquipped = false;

	bKnocked = false;

	//bSteal = false;
	

	bSteal = false;

	bisLocked = false;
	//overlappingBall->SetBallState(EBallState::EBS_Dropped);
	//testVect = combat->character->GetActorForwardVector() -/*lockly->GetTarget()->GetActorForwardVector()*/;
	UE_LOG(LogTemp, Warning, TEXT("The vector value is: %s"), *testVect.ToString());

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	
}
void ACPPTestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//important to include #include "Net/UnrealNetwork.h" whenever we use the replication macro
	DOREPLIFETIME_CONDITION(ACPPTestCharacter, overlappingBall, COND_OwnerOnly);//Replication

	DOREPLIFETIME(ACPPTestCharacter, bEquipped);

	DOREPLIFETIME(ACPPTestCharacter, bCatching);
	DOREPLIFETIME(ACPPTestCharacter, bSteal);

	DOREPLIFETIME(ACPPTestCharacter, bisLocked);
	DOREPLIFETIME(ACPPTestCharacter, bStunned);
	DOREPLIFETIME(ACPPTestCharacter, bKnocked);
	DOREPLIFETIME(ACPPTestCharacter, CurrentHealth);
	DOREPLIFETIME(ACPPTestCharacter, ballHitDirection);

	DOREPLIFETIME(ACPPTestCharacter, throwPower);

}


void ACPPTestCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (combat)
	{
		combat->character = this;
	}
}

void ACPPTestCharacter::Jump()
{
	if (IsAllowedToMove()) {
		Super::Jump();
	}
}


void ACPPTestCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateHUDHealth();
	if (HasAuthority()) {
		CharacterMesh->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
		//OnTakeAnyDamage.AddDynamic(this, &ACPPTestCharacter::TakeDamage);
		CPPPlayerController = Cast<ACPPPlayerController>(Controller);

	} 
	
}
void ACPPTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void ACPPTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ACPPTestCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("LockOn", IE_Pressed, this, &ACPPTestCharacter::LockTarget);


	PlayerInputComponent->BindAction("Catch", IE_Pressed, this, &ACPPTestCharacter::Catch);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ACPPTestCharacter::Dash);

	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &ACPPTestCharacter::ThrowButtonPressed);
	PlayerInputComponent->BindAction("Throw", IE_Released, this, &ACPPTestCharacter::ThrowButtonReleased);



	PlayerInputComponent->BindAxis("MoveForward", this, &ACPPTestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACPPTestCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ACPPTestCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ACPPTestCharacter::LookUp);


}

#pragma region Movement and Dashing
void ACPPTestCharacter::MoveForward(float value)
{
	/*if (Controller != nullptr && value != 0.0f && IsAllowedToMove())
	{
		const FRotator yawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(direction, value);
	}*/

	AddMovementInput(GetActorForwardVector() * value);
}

void ACPPTestCharacter::MoveRight(float value)
{
	/*if (Controller != nullptr && value != 0.0f && IsAllowedToMove())
	{
		const FRotator yawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(direction, value);
	}*/

	AddMovementInput(GetActorRightVector() * value);
}

void ACPPTestCharacter::Turn(float value)
{
	//AddControllerYawInput(value);

#pragma region TargetSwitching
	
	if (lockOnTargets->IsTargetLocked())
	{
		lockOnTargets->SwitchTargetYaw(value);
	}
	else
	{
		lockOnTargets->SwitchTargetPitch(value);
	}
#pragma endregion

	AddControllerYawInput(value * 20 * GetWorld()->GetDeltaSeconds());
}

void ACPPTestCharacter::LookUp(float value)
{
	//AddControllerPitchInput(value);

#pragma region TargetSwitching
	
	if (lockOnTargets->IsTargetLocked())
	{
		lockOnTargets->SwitchTargetPitch(value * -1.0f);
	}
	else
	{
		AddControllerPitchInput(value);
	}
#pragma endregion

	AddControllerPitchInput(value * 20 * GetWorld()->GetDeltaSeconds());
}

// Lock on Target 
void ACPPTestCharacter::LockTarget()
{
	lockOnTargets->EnableTargeting();
}

void ACPPTestCharacter::Dash()
{

	if (HasAuthority())
	{
		if (bCanDash && CanJump() && IsAllowedToMove()) {



			if (DashAnim)
			{
				MulticastPlayAnimMontage(DashAnim, 1, NAME_None);

			}

			const FVector ForwardVector = this->GetActorRotation().Vector();
			LaunchCharacter(ForwardVector * DashDistance, true, true);

			bCanDash = false;

			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::CanDash, 1.f);
		}
	}
	else
	{
		DashButtonPressed();
	}

}


void ACPPTestCharacter::CanDash()
{
	bCanDash = true;
}

void ACPPTestCharacter::CanCatch()
{
	bCatching = false;
}

void ACPPTestCharacter::DashButtonPressed_Implementation()
{
	if (bCanDash && CanJump() && IsAllowedToMove()) {

		if (DashAnim)
		{
			PlayAnimMontage(DashAnim, 1, NAME_None);
			ServerPlayAnimMontage(DashAnim, 1, NAME_None);

		}

		const FVector ForwardVector = this->GetActorRotation().Vector();
		LaunchCharacter(ForwardVector * DashDistance, true, true);

		bCanDash = false;

		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::CanDash, 1.f);
	}
}
#pragma endregion

#pragma region BallEquipping
void ACPPTestCharacter::EquipButtonPressed()
{
	if (combat && overlappingBall)
	{
		if (HasAuthority())
		{
			bEquipped = true;
			combat->EquipBall(overlappingBall);
			if (combat->equippedBall->GetBallState() == EBallState::EBS_Dropped)
				bThrown = false;
			if (combat->equippedBall->GetBallState() == EBallState::EBS_Equipped)
				bCanThrow = true;
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}
void ACPPTestCharacter::ServerEquipButtonPressed_Implementation() //RPC
{
	if (combat)
	{
		bEquipped = true;

		combat->EquipBall(overlappingBall);
		if (combat->equippedBall->GetBallState() == EBallState::EBS_Dropped)
			bThrown = false;
		if (combat->equippedBall->GetBallState() == EBallState::EBS_Equipped)
			bCanThrow = true;

	}
}
#pragma endregion

//	Throw Mechanics TODO
//		-> Throw ball on target ??

#pragma region ThrowMechanics
void ACPPTestCharacter::ThrowButtonPressed()
{
	if (combat)
	{
		if (HasAuthority())
		{
			combat->ThrowButtonPressed(true);
			if (combat && bEquipped)
			{
				MyThrow();	
			}	
		}
		else
		{
			ServerThrowButtonPressed();
		}
	}
}

void ACPPTestCharacter::ClientRespawnCountDown_Implementation(float seconds)
{
	if (seconds  > 0.0 && bKnocked)
	{
		RespawingWidget = CreateWidget<UUI_RespawnWidget>(GetLocalViewingPlayerController(), RespawingCountWidgetClass);
		RespawingWidget->CountdownTimeSeconds = seconds;
		RespawingWidget->AddToViewport();

		FTimerHandle RespawnCountHandle;
		GetWorldTimerManager().SetTimer(RespawnCountHandle, this, &ThisClass::RemoveWidget, seconds, false);
	}
}

void ACPPTestCharacter::ServerThrowButtonPressed_Implementation()
	{
		if (combat && IsBallEquipped())
		{
			combat->ThrowButtonPressed(true);
			
			if (combat && bEquipped)
			{
				MyThrow();
			}
		}
	}

void ACPPTestCharacter::ThrowButtonReleased()
{
}
void ACPPTestCharacter::ServerThrowButtonReleased_Implementation()
{
}


void ACPPTestCharacter::Stunned()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,
			5.f, FColor::Green, FString::Printf(TEXT("STUN")));
	}
	bStunned = true;

	FTimerHandle StunHandle;
	GetWorld()->GetTimerManager().SetTimer(StunHandle, this, &ThisClass::StunCoolDown, StunDuration);
}

void ACPPTestCharacter::StunCoolDown()
{
	bStunned = false;
}

void ACPPTestCharacter::OnHealthUpdate()
{
	
	if (IsLocallyControlled())
	{
		

		if (CurrentHealth <= 0)
		{

			/*if (IsBallEquipped())
			{
				this->combat->UnEquipBall(combat->equippedBall);
			}
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);

			
			//Knocked();
			Knocked();*/
			

		}
		
	}

	//Server-specific functionality
	if (GetLocalRole() == ROLE_Authority)
	{

		if (CurrentHealth <= 0)
		{
			if (IsBallEquipped())
			{
				combat->UnEquipBall(combat->equippedBall);
			}
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);

			Knocked();
		}
	}
}

void ACPPTestCharacter::UpdateHUDHealth()
{
	CPPPlayerController = CPPPlayerController == nullptr ? Cast<ACPPPlayerController>(Controller) : CPPPlayerController;
	MyPlayerState = MyPlayerState == nullptr ? Cast<AMyPlayerState>(MyPlayerState) : MyPlayerState;

	if (CPPPlayerController)
	{
		CPPPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
	}
}

void ACPPTestCharacter::Elim()
{

}

void ACPPTestCharacter::Knocked()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		MultiKnocked();

		FTimerHandle KnockedTimerDestroy;

		GetWorld()->GetTimerManager().SetTimer(KnockedTimerDestroy, this, &ACPPTestCharacter::CallDestroy, 7.0f, false);

		AMyGameMode* Mode = GetWorld()->GetAuthGameMode<AMyGameMode>();

		if (AMyGameMode* FreeforallMode = Cast<AMyGameMode>(Mode))
		{

			FreeforallMode->Respawn(GetController());
			
		}
	}

}

bool ACPPTestCharacter::MultiKnocked_Validate()
{
	return true;
}

void ACPPTestCharacter::MultiKnocked_Implementation()
{
	bKnocked = true;
	this->GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	this->GetMesh()->SetAllBodiesSimulatePhysics(true);
	this->GetMesh()->AddImpulse( GetActorLocation() + (-ballHitDirection * HitImpulse * CharacterMesh->GetMass()));

}

void ACPPTestCharacter::ElimTimerFinished()
{
	AMyGameMode* MyGameMode = GetWorld()->GetAuthGameMode<AMyGameMode>();
	if (MyGameMode)
	{
	//MyGameMode->RequestRespawn(this, Controller);
	}
}

#pragma endregion


void ACPPTestCharacter::Catch()
{
	//TODO
	// -> implement Catch Mechanics

	if (!bCatching && IsAllowedToMove()) {

		if (HasAuthority())
		{

			if (CatchAnim)
			{
				MulticastPlayAnimMontage(CatchAnim, 1, NAME_None);
				//bEquipped = true;


			}
			bCatching = true;
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACPPTestCharacter::CanCatch, CatchCooldown);

		}
		else
		{
			ServerCatch();
		}
	}
}

void ACPPTestCharacter::ServerCatch_Implementation()
{
	//TODO
	// -> Catch Replication (must be fixed)
	
		if (CatchAnim)
		{
			PlayAnimMontage(CatchAnim, 1, NAME_None);
			ServerPlayAnimMontage(CatchAnim, 1, NAME_None);
			//bEquipped = true;

		}

		bCatching = true;
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACPPTestCharacter::CanCatch, CatchCooldown);
	
}

void ACPPTestCharacter::OnBallReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("md5lsh"));
	if (combat)
	{
		UE_LOG(LogTemp, Warning, TEXT("d5l ?"));
	}
}


void ACPPTestCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// BallHit;
	//TODO: overlapping ball to be removed and/or change name

	if (ACPPBall* BallHit = Cast<ACPPBall>(OtherActor))
	{

		if (bCatching && combat && !IsBallEquipped() || bSteal)
		{
			combat->EquipBall(BallHit);
			bCanThrow = true;
			bSteal = false;
			
			//bSteal = false;



		}

		else if (BallHit->GetBallState() == EBallState::EBS_Dropped && BallHit->GetOwner() != this)
		{

			FString HitMessage = FString::Printf(TEXT("HIT"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, HitMessage);
			FDamageEvent GotHitEvent;
			this->TakeDamage(50.f, GotHitEvent, BallHit->GetInstigatorController(), BallHit);
			BallHit->SetBallState(EBallState::EBS_Initial);
			ballHitDirection = BallHit->GetActorForwardVector();
			BallHit->SetOwner(nullptr);

		} else if (BallHit->GetBallState() == EBallState::EBS_Initial && combat && !IsBallEquipped())
		{
			combat->EquipBall(BallHit);
		}
	}
}


void ACPPTestCharacter::MyThrow()
{
	if (combat && IsBallEquipped() && IsAllowedToMove())
	{
		UKismetMathLibrary::GetForwardVector(GetControlRotation()) *= throwPower;
		//const FVector forwardVec = this->GetMesh()->GetForwardVector();
		combat->ThrowButtonPressed(false); //gded
		combat->equippedBall->GetBallMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		combat->equippedBall->SetBallState(EBallState::EBS_Dropped);
		bThrown = true;
		combat->equippedBall->GetBallMesh()->SetSimulatePhysics(true);

		if (lockOnTargets->GetTarget())
		{
		combat->equippedBall->GetBallMesh()->AddForce((((lockOnTargets->GetTarget()->GetTargetLocation()) - (GetActorLocation())) + GetActorUpVector()) * throwPower * 25);
		}
		else
		{
		combat->equippedBall->GetBallMesh()->AddForce((GetActorForwardVector() + GetActorUpVector()) * throwPower * 2500);
		}
		bEquipped = false;
		combat->equippedBall = nullptr;
		OnBallReleased();
	}	
}

void ACPPTestCharacter::SetOverlappingBall(ACPPBall* cppBall)
{
	if (overlappingBall)
	{
		overlappingBall->ShowPickupWidget(false);
	}
	overlappingBall = cppBall;
	if (IsLocallyControlled())
	{
		if (overlappingBall)
		{
			overlappingBall->ShowPickupWidget(true);
		}
	}
}

void ACPPTestCharacter::CallDestroy()
{
	Destroy();
	GetCapsuleComponent()->DestroyComponent();

}

void ACPPTestCharacter::OnRep_CurrentHealth()
{
	UpdateHUDHealth();
	OnHealthUpdate();

}

void ACPPTestCharacter::OnRep_OverlappingBall(ACPPBall* lastBall)
{
	if (overlappingBall)
	{
		overlappingBall->ShowPickupWidget(true);
	}
	if (lastBall)
	{
		lastBall->ShowPickupWidget(false);
	}
}

#pragma region Animations
bool ACPPTestCharacter::IsBallEquipped()
{
	return (combat && combat->equippedBall);
}

void ACPPTestCharacter::StopThrow()
{
	bThrown = false;
	bCanThrow = false;
}

bool ACPPTestCharacter::IsAllowedToMove()
{
	return !bStunned && !bKnocked;

}

void ACPPTestCharacter::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float ACPPTestCharacter::TakeDamage(float DamageTaken, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	
	UpdateHUDHealth();
	float DamageApplied = CurrentHealth - DamageTaken;
	SetCurrentHealth(DamageApplied);
	CPPPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
	if (CurrentHealth == 0.0f)
	{
		AMyGameMode* MyGameMode = GetWorld()->GetAuthGameMode<AMyGameMode>();
		if (MyGameMode)
		{
			FString HitMessage = FString::Printf(TEXT("D5l wala la2 ?"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, HitMessage);
			CPPPlayerController = CPPPlayerController == nullptr ? Cast<ACPPPlayerController>(Controller) : CPPPlayerController;
			ACPPPlayerController* AttackerController = Cast<ACPPPlayerController>(EventInstigator);
			MyGameMode->PlayerEliminated(this, CPPPlayerController, AttackerController);
		}
	}
	return DamageApplied;

}

void ACPPTestCharacter::RemoveWidget()
{
	if (RespawingWidget)
	{
		RespawingWidget->RemoveFromViewport();
		RespawingWidget = nullptr;
	}
}

void ACPPTestCharacter::PlayThrowMontage()
{
	if (combat == nullptr || combat->equippedBall == nullptr) return;

	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && throwAnim)
	{
		animInstance->Montage_Play(throwAnim);
		ServerPlayAnimMontage(throwAnim);
	}
}

void ACPPTestCharacter::ServerPlayAnimMontage_Implementation(UAnimMontage* AnimMontage, float InPlayRate,
	FName StartSectionName)
{
	MulticastPlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}

bool ACPPTestCharacter::ServerPlayAnimMontage_Validate(UAnimMontage* AnimMontage, float InPlayRate,
	FName StartSectionName)
{
	return true;
}

void ACPPTestCharacter::MulticastPlayAnimMontage_Implementation(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}

bool ACPPTestCharacter::MulticastPlayAnimMontage_Validate(UAnimMontage* AnimMontage, float InPlayRate,
	FName StartSectionName)
{
	return true;
}



#pragma endregion



