

#include "CPPTestCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h" //Replication
#include "testinginBP\Ball\CPPBall.h"
#include "testinginBP\Ball\BallSpawner.h"
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
#include "PowerCharacter.h"
#include "testinginBP/PlayerController/CPPPlayerController.h"
#include "testinginBP/GameComponents/MyPlayerState.h"
#include "testinginBP/GameMode/MyGameMode.h"

#include "TimerManager.h"

ACPPTestCharacter::ACPPTestCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CharacterMesh = GetMesh();

	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	cameraBoom->SetupAttachment(GetMesh());
	cameraBoom->TargetArmLength = 500.0f;

	cameraBoom->SetRelativeRotation(FRotator(0, 90, 0));
	cameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	cameraBoom->bUsePawnControlRotation = true;

	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);
	
	DashFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DashFX"));
	DashFX->SetupAttachment(GetMesh());

	/*overHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	overHeadWidget->SetupAttachment(RootComponent);*/

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

	bStunned = false;

	bCanDash = true;

	bThrown = false;

	bCanThrow = false;

	bEquipped = false;

	bKnocked = false;

	bSteal = false;

	bisLocked = false;
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
	DOREPLIFETIME(ACPPTestCharacter, MaxHealth);
	DOREPLIFETIME(ACPPTestCharacter, ballHitDirection);
	DOREPLIFETIME(ACPPTestCharacter, throwPower);
	DOREPLIFETIME(ACPPTestCharacter, bIsDashing);
	DOREPLIFETIME(ACPPTestCharacter, bIsSpawnInvincible);
	DOREPLIFETIME(ACPPTestCharacter, bIsTargeting);

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
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
		CPPPlayerController = Cast<ACPPPlayerController>(Controller);

	}

	bIsSpawnInvincible = true;
	FTimerHandle InvincibleHandle;
	GetWorld()->GetTimerManager().SetTimer(InvincibleHandle, this, &ThisClass::SetSpawnInvincibility, SpawnInvincibilityDuration);

	if(InvincibleMaterialInstance)
	{
		OriginalMat1 = GetMesh()->GetMaterial(0);
		OriginalMat2 = GetMesh()->GetMaterial(1);
		OriginalMat3 = GetMesh()->GetMaterial(2);

		DynamicInvincibleMatInst = UMaterialInstanceDynamic::Create(InvincibleMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicInvincibleMatInst);
		GetMesh()->SetMaterial(1, DynamicInvincibleMatInst);
		GetMesh()->SetMaterial(2, DynamicInvincibleMatInst);

		
	}
	
}

void ACPPTestCharacter::Falling()
{
	Super::Falling();

	//bCanDash = false;
	//FTimerHandle DashHandle;
	//GetWorld()->GetTimerManager().SetTimer(DashHandle, this, &ThisClass::CanDash, DashCoolDownDuration);
}

void ACPPTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PollInit();

	if (bIsTargeting && lockOnTargets->bCanCaptureTarget)
	{
		lockOnTargets->FindTarget();
	}

}
void ACPPTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("LockOn", IE_Pressed, this, &ACPPTestCharacter::LockTarget);


	PlayerInputComponent->BindAction("Catch", IE_Pressed, this, &ACPPTestCharacter::Catch);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ACPPTestCharacter::Dash);

	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &ACPPTestCharacter::ThrowButtonPressed);
	PlayerInputComponent->BindAction("Throw", IE_Released, this, &ACPPTestCharacter::ThrowButtonReleased);



	PlayerInputComponent->BindAxis("MoveForward", this, &ACPPTestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACPPTestCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ACPPTestCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ACPPTestCharacter::LookUp);

	PlayerInputComponent->BindAction("SpawnBall", IE_Pressed, this, &ThisClass::SpawnActors);


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

	if (IsAllowedToMove()) {
		if(value < 0 && !bIsDashing)
		{
			AddMovementInput(GetActorForwardVector() * (value * 0.7f));
		} else
		{
			
			AddMovementInput(GetActorForwardVector() * value);
		}
	}
}

void ACPPTestCharacter::MoveRight(float value)
{
	/*if (Controller != nullptr && value != 0.0f && IsAllowedToMove())
	{
		const FRotator yawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(direction, value);
	}*/

	if (IsAllowedToMove()) {
		if(!bIsDashing)
		{
			AddMovementInput(GetActorRightVector() * (value * 0.7f));
		} else
		{
			AddMovementInput(GetActorRightVector() * value);
		}
	}
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
	bIsTargeting = true;
	lockOnTargets->EnableTargeting();
}

void ACPPTestCharacter::Dash()
{

	if (HasAuthority())
	{
		if (bCanDash && CanJump() && IsAllowedToMove() && GetCharacterMovement()->IsMovingOnGround()) {

			GetCharacterMovement()->FallingLateralFriction = 3;
			if (GetMovementComponent()->GetLastInputVector() != FVector::ZeroVector)
			{
				const FVector ForwardVector = GetMovementComponent()->GetLastInputVector();
				LaunchCharacter(ForwardVector * DashDistance, true, true);

			}
			else
				LaunchCharacter(GetActorForwardVector() * DashDistance, true, true);
			/*if (DashAnim)
			{
				MulticastPlayAnimMontage(DashAnim, 1, NAME_None);

			}*/
			MulticastPlayNiagara(DashFX, true);

			bCanDash = false;
			bIsDashing = true;
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::CanDash, DashCoolDownDuration);

			FTimerHandle AnimHandle;
			GetWorld()->GetTimerManager().SetTimer(AnimHandle, this, &ThisClass::SetDashingAnimOff, DashAnimDuration);
		}
	}
	else
	{
		DashButtonPressed(GetLastMovementInputVector());
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

void ACPPTestCharacter::DashButtonPressed_Implementation(FVector DashDir)
{
	if (bCanDash && CanJump() && IsAllowedToMove() && GetCharacterMovement()->IsMovingOnGround()) {

		//if (DashAnim)
		//{
		//	PlayAnimMontage(DashAnim, 1, NAME_None);
		//	MulticastPlayAnimMontage(DashAnim, 1, NAME_None); //todo: MultiStartedDash();
		//}

		MulticastPlayNiagara(DashFX, true);
		
		GetCharacterMovement()->FallingLateralFriction = 3;
		if (DashDir != FVector::ZeroVector)
		{
			LaunchCharacter(DashDir * DashDistance, true, true);

		}
		else
			LaunchCharacter(GetActorForwardVector() * DashDistance, true, true);
		

		bCanDash = false;
		bIsDashing = true;
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::CanDash, DashCoolDownDuration);

		FTimerHandle AnimHandle;
		GetWorld()->GetTimerManager().SetTimer(AnimHandle, this, &ThisClass::SetDashingAnimOff, DashAnimDuration);
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

void ACPPTestCharacter::ClientRespawnCountDown_Implementation(float seconds)
{
	if (seconds > 0.0 && bKnocked)
	{
		RespawingWidget = CreateWidget<UUI_RespawnWidget>(GetLocalViewingPlayerController(), RespawingCountWidgetClass);
		RespawingWidget->CountdownTimeSeconds = seconds;
		RespawingWidget->AddToViewport();

		
		FTimerHandle RespawnCountHandle ;
		FTimerDelegate RespawnCountDelegate;
		RespawnCountDelegate.BindUFunction(this, FName("RemoveWidget"), RespawingWidget);
		GetWorldTimerManager().SetTimer(
			RespawnCountHandle,
			RespawnCountDelegate, 
			seconds, 
			false
		);

	}
}
//	Throw Mechanics TODO
//		-> Throw ball on target ??

#pragma region ThrowMechanics
void ACPPTestCharacter::ThrowButtonPressed()
{
	if(IsBallEquipped())
	{
		
		LockTarget();
	}
}


void ACPPTestCharacter::ServerThrowButtonPressed_Implementation()
	{
		
	}

void ACPPTestCharacter::ThrowButtonReleased()
{
	if (combat && IsBallEquipped())
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
			ServerThrowButtonReleased();
		}

		ClearTarget();
	}
}
void ACPPTestCharacter::ServerThrowButtonReleased_Implementation()
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


void ACPPTestCharacter::ServerLeaveGame_Implementation()
{
	AMyGameMode* GameMode = GetWorld()->GetAuthGameMode<AMyGameMode>();
	MyPlayerState = MyPlayerState == nullptr ? GetPlayerState<AMyPlayerState>() : MyPlayerState;
	if (GameMode && MyPlayerState)
	{
		GameMode->PlayerLeftGame(MyPlayerState);
	}
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

			Knocked(ballHitDirection,bLeftGame);
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

void ACPPTestCharacter::Knocked(FVector ImpulseDirection,bool bPlayerLeftGame)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		MultiKnocked(ImpulseDirection, bPlayerLeftGame);

		FTimerHandle KnockedTimerDestroy;

		GetWorld()->GetTimerManager().SetTimer(KnockedTimerDestroy, this, &ACPPTestCharacter::CallDestroy, 6.0f, false);

		AMyGameMode* Mode = GetWorld()->GetAuthGameMode<AMyGameMode>();
		AMyGameMode* FreeforallMode = Cast<AMyGameMode>(Mode);

		if (FreeforallMode && !bLeftGame)
		{

			FreeforallMode->Respawn(GetController());
		
		}
		if (bLeftGame && IsLocallyControlled())
		{
			OnLeftGame.Broadcast();
		}
	}

}

bool ACPPTestCharacter::MultiKnocked_Validate(FVector ImpulseDirection,bool bPlayerLeftGame)
{
	return true;
}

void ACPPTestCharacter::MultiKnocked_Implementation(FVector ImpulseDirection,bool bPlayerLeftGame)
{
	bLeftGame = bPlayerLeftGame;
	bKnocked = true;
	this->CharacterMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	this->CharacterMesh->SetAllBodiesSimulatePhysics(true);
	this->CharacterMesh->AddImpulse(GetActorLocation() + ((ImpulseDirection + GetActorUpVector()) * HitImpulse * CharacterMesh->GetMass()));
	this->CharacterMesh->SetEnableGravity(false);

}

void ACPPTestCharacter::PollInit()
{
	if (MyPlayerState == nullptr)
	{
		MyPlayerState = GetPlayerState<AMyPlayerState>();
		if (MyPlayerState)
		{
			MyPlayerState->AddToScore(0.f);
			MyPlayerState->AddToDefeats(0);
		}
	}
}

void ACPPTestCharacter::ClearTarget()
{

	bIsTargeting = false;
	lockOnTargets->ClearTargetManual();
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
	//if (bDisableGameplay) return;
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


	if (ACPPBall* BallHit = Cast<ACPPBall>(OtherActor))
	{

		if (bCatching && combat && !IsBallEquipped() || bSteal)
		{
			combat->EquipBall(BallHit);
			bCanThrow = true;
			bSteal = false;
		}

		else if ((BallHit->GetBallState() == EBallState::EBS_Dropped ||
			BallHit->GetBallState() == EBallState::EBS_SuperThrow) && BallHit->GetOwner() != this)
		{
			if (!bIsSpawnInvincible) {

				FDamageEvent GotHitEvent;
				AController* ballOwnerController = BallHit->GetInstigatorController();
				ballHitDirection = ballOwnerController->GetCharacter()->GetActorForwardVector();
				
				if (BallHit->GetBallState() == EBallState::EBS_SuperThrow)
				{
					Knocked(ballHitDirection, false);
					
				}
				else
				{
					UGameplayStatics::ApplyDamage(this, 50.f, ballOwnerController, BallHit, NULL);
				}
				
				if (CurrentHealth > 0 && GetHitAnim) {
					MulticastPlayAnimMontage(GetHitAnim, 1, NAME_None);
				}
			}
			//this->TakeDamage(50.f, GotHitEvent, BallHit->GetInstigatorController(), BallHit);
			BallHit->SetBallState(EBallState::EBS_Initial);
			//TODO : Make A reset function for owner and instigator
			BallHit->SetOwner(nullptr);
			BallHit->SetInstigator(nullptr);


		}
		else if (BallHit->GetBallState() == EBallState::EBS_Initial && combat && !IsBallEquipped())
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
		 //gded
		//combat->equippedBall->GetBallMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		combat->equippedBall->GetAreaSphere()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		if (combat->equippedBall->GetBallState() != EBallState::EBS_SuperThrow) {
			combat->equippedBall->SetBallState(EBallState::EBS_Dropped);
			combat->ThrowButtonPressed(false);
		}
		bThrown = true;
		//combat->equippedBall->GetBallMesh()->SetSimulatePhysics(true);
		combat->equippedBall->GetAreaSphere()->SetSimulatePhysics(true);
		

		if (lockOnTargets->GetTarget())
		{
		//combat->equippedBall->GetBallMesh()->AddForce((((lockOnTargets->GetTarget()->GetTargetLocation()) - (GetActorLocation())) + GetActorUpVector()) * throwPower * 25);
		combat->equippedBall->GetAreaSphere()->AddForce((((lockOnTargets->GetTarget()->GetTargetLocation()) - (GetActorLocation())) + GetActorUpVector()) * throwPower * 25);

		}
		else
		{
		//combat->equippedBall->GetBallMesh()->AddForce((GetActorForwardVector() + GetActorUpVector()) * throwPower * 2500);
		combat->equippedBall->GetAreaSphere()->AddForce((GetActorForwardVector() + GetActorUpVector()) * throwPower * 2500);

		}
		bEquipped = false;
		combat->equippedBall->AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

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

void ACPPTestCharacter::ResetHealthHUD(float timer)
{
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::ResetHealthTest, timer);

}
void ACPPTestCharacter::ResetHealthTest()
{
	if(HasAuthority())
	CPPPlayerController->SetHUDHealth(100.0F, MaxHealth);
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

void ACPPTestCharacter::SetSpawnInvincibility()
{
	bIsSpawnInvincible = false;

	GetMesh()->SetMaterial(0, OriginalMat1);
	GetMesh()->SetMaterial(1, OriginalMat2);
	GetMesh()->SetMaterial(2, OriginalMat3);

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
			CPPPlayerController = CPPPlayerController == nullptr ? Cast<ACPPPlayerController>(Controller) : CPPPlayerController; 
			ACPPTestCharacter* OwnerCharacter = Cast<ACPPTestCharacter>(GetOwner());
			ACPPPlayerController* DamageCauserController = Cast<ACPPPlayerController>(EventInstigator);
			MyGameMode->PlayerEliminated(this, CPPPlayerController, DamageCauserController);
		}
	}
	return DamageApplied;

}

void ACPPTestCharacter::RemoveWidget(UUI_RespawnWidget* MsgToRemove)
{
	if (MsgToRemove)
	{
		FString lol = FString::Printf(TEXT("LOL mn el widget"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, lol);

		MsgToRemove->RemoveFromParent();
	}
		
		//RespawingWidget->RemoveFromViewport();
}

void ACPPTestCharacter::SetDashingAnimOff()
{
	bIsDashing = false;
	if (DashFX) {
		MulticastPlayNiagara(DashFX, false);
	}
}

void ACPPTestCharacter::SpawnActors()
{
	//Find the Actor Spawner in the world, and invoke it's Spawn Actor function
	AActor* ActorSpawnerTofind = UGameplayStatics::GetActorOfClass(GetWorld(), ABallSpawner::StaticClass());

	ABallSpawner* ActorSpawnerReference = Cast<ABallSpawner>(ActorSpawnerTofind);
	if (ActorSpawnerReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found"));
		ActorSpawnerReference->SpawnActor();
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
		MulticastPlayAnimMontage(throwAnim);
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


void ACPPTestCharacter::MulticastPlayNiagara_Implementation(UNiagaraComponent* fx, bool state)
{
	
	if (!state)
	{
		fx->Deactivate();
	} else
	{
		fx->Activate(state);
	}
}

bool ACPPTestCharacter::MulticastPlayNiagara_Validate(UNiagaraComponent* fx, bool state)
{
	return true;
}

void ACPPTestCharacter::ServerPlayNiagara_Implementation(UNiagaraComponent* fx, bool state)
{
	MulticastPlayNiagara(fx, state);
}

bool ACPPTestCharacter::ServerPlayNiagara_Validate(UNiagaraComponent* fx, bool state)
{
	return true;
}

#pragma endregion



