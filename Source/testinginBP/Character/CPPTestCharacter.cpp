

#include "CPPTestCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h" //Replication
#include "testinginBP\Ball\CPPBall.h"
//#include "testinginBP\Ball\BallSpawner.h"
#include "testinginBP\GameComponents\CombatComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "testinginBP\Character\CPPAnimInstance.h"
#include "LockOnTargetComponent.h"
#include "LockOnSubobjects/TargetHandlers/TargetHandlerBase.h"
#include "TargetingHelperComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
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

	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);

	DashFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DashFX"));
	DashFX->SetupAttachment(GetMesh());

	DustFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DustFX"));
	DustFX->SetupAttachment(GetMesh());

	LockFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LockFX"));
	LockFX->SetupAttachment(GetMesh());

	StunFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("StunFX"));
	StunFX->SetupAttachment(GetMesh());

	lockOnTargets = CreateDefaultSubobject<ULockOnTargetComponent>(TEXT("LockOnTarget"));
	lockOnTargets->SetIsReplicated(true);

	targetComponent = CreateDefaultSubobject<UTargetingHelperComponent>(TEXT("targetComponent"));
	targetComponent->SetIsReplicated(true);

	HitEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HitEffect"));
	HitEffect->SetupAttachment(GetMesh());

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
	bDoingAbility = false;
	bisLocked = false;
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

}

void ACPPTestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(ACPPTestCharacter, overlappingBall, COND_OwnerOnly);//Replication
	DOREPLIFETIME(ACPPTestCharacter, bEquipped);
	DOREPLIFETIME(ACPPTestCharacter, bCatching);
	DOREPLIFETIME(ACPPTestCharacter, bSteal);
	DOREPLIFETIME(ACPPTestCharacter, bisLocked);
	DOREPLIFETIME(ACPPTestCharacter, bStunned);
	DOREPLIFETIME(ACPPTestCharacter, bKnocked);
	DOREPLIFETIME(ACPPTestCharacter, CurrentHealth);
	DOREPLIFETIME(ACPPTestCharacter, MaxHealth);
	DOREPLIFETIME(ACPPTestCharacter, ballHitDirection);
	DOREPLIFETIME(ACPPTestCharacter, bIsDashing);
	DOREPLIFETIME(ACPPTestCharacter, bIsSpawnInvincible);
	DOREPLIFETIME(ACPPTestCharacter, bIsTargeting);
	DOREPLIFETIME(ACPPTestCharacter, bDoingAbility);

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

	if (InvincibleMaterialInstance)
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
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ACPPTestCharacter::Dash);

	PlayerInputComponent->BindAction("Emote", IE_Pressed, this, &ACPPTestCharacter::DoEmote);

	PlayerInputComponent->BindAction("Catch", IE_Pressed, this, &ACPPTestCharacter::Catch);
	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &ACPPTestCharacter::ThrowButtonPressed);
	PlayerInputComponent->BindAction("Throw", IE_Released, this, &ACPPTestCharacter::ThrowButtonReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACPPTestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACPPTestCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ACPPTestCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ACPPTestCharacter::LookUp);

	//PlayerInputComponent->BindAction("SpawnBall", IE_Pressed, this, &ThisClass::SpawnActors);

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
		if (value < 0 && !bIsDashing)
		{
			AddMovementInput(GetActorForwardVector() * (value * BackwardMultiplier));
			if (DustFX) {
				ServerPlayNiagara(DustFX, false);
				MulticastPlayNiagara(DustFX, false);
			}
		}
		else
		{
			if (DustFX) {
				ServerPlayNiagara(DustFX, true);
				MulticastPlayNiagara(DustFX, true);
			}
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
		if (!bIsDashing)
		{
			AddMovementInput(GetActorRightVector() * (value * StrafeMultiplier));
			if (DustFX) {
				ServerPlayNiagara(DustFX, false);
				MulticastPlayNiagara(DustFX, false);
			}

		}
		else
		{
			AddMovementInput(GetActorRightVector() * value);
			if (DustFX) {
				ServerPlayNiagara(DustFX, false);
				MulticastPlayNiagara(DustFX, false);
			}
		}
	}
}

void ACPPTestCharacter::Turn(float value)
{

#pragma region TargetSwitching

	if (lockOnTargets->IsTargetLocked())
	{
		lockOnTargets->SwitchTargetYaw(value);
	}
	else
	{
		AddControllerYawInput(value * SensetivityX * GetWorld()->GetDeltaSeconds());
	}
#pragma endregion


}

void ACPPTestCharacter::LookUp(float value)
{

#pragma region TargetSwitching

	if (lockOnTargets->IsTargetLocked())
	{
		lockOnTargets->SwitchTargetPitch(value * -1.0f);
	}
	else
	{
		AddControllerPitchInput(value * SensetivityY * GetWorld()->GetDeltaSeconds());
	}
#pragma endregion
}

void ACPPTestCharacter::LockTarget()
{
	bIsTargeting = true;
	lockOnTargets->EnableTargeting();
}

void ACPPTestCharacter::Dash()
{

	if (HasAuthority())
	{
		if (bCanDash && IsAllowedToMove()) {

			GetCharacterMovement()->FallingLateralFriction = 4;
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
	if (bCanDash && IsAllowedToMove()) {

		//if (DashAnim)
		//{
		//	PlayAnimMontage(DashAnim, 1, NAME_None);
		//	MulticastPlayAnimMontage(DashAnim, 1, NAME_None); //todo: MultiStartedDash();
		//}

		MulticastPlayNiagara(DashFX, true);

		GetCharacterMovement()->FallingLateralFriction = 4;
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
//void ACPPTestCharacter::EquipButtonPressed()
//{
//	if (combat && overlappingBall)
//	{
//		if (HasAuthority())
//		{
//			bEquipped = true;
//			combat->EquipBall(overlappingBall);
//			if (combat->equippedBall->GetBallState() == EBallState::EBS_Dropped)
//				bThrown = false;
//			if (combat->equippedBall->GetBallState() == EBallState::EBS_Equipped)
//				bCanThrow = true;
//		}
//		else
//		{
//			ServerEquipButtonPressed();
//		}
//	}
//}
//void ACPPTestCharacter::ServerEquipButtonPressed_Implementation() //RPC
//{
//	if (combat)
//	{
//		bEquipped = true;
//
//		combat->EquipBall(overlappingBall);
//		if (combat->equippedBall->GetBallState() == EBallState::EBS_Dropped)
//			bThrown = false;
//		if (combat->equippedBall->GetBallState() == EBallState::EBS_Equipped)
//			bCanThrow = true;
//
//	}
//}
#pragma endregion

void ACPPTestCharacter::ClientRespawnCountDown_Implementation(float seconds)
{
	if (seconds > 0.0 && bKnocked)
	{
		RespawingWidget = CreateWidget<UUI_RespawnWidget>(GetLocalViewingPlayerController(), RespawingCountWidgetClass);
		RespawingWidget->CountdownTimeSeconds = seconds;
		RespawingWidget->AddToViewport();


		FTimerHandle RespawnCountHandle;
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

#pragma region ThrowMechanics
void ACPPTestCharacter::ThrowButtonPressed()
{
	if (IsBallEquipped() && !bDoingAbility)
	{
		LockTarget();

		if (ACPPTestCharacter* TargetPlayer = Cast<ACPPTestCharacter>(lockOnTargets->GetTarget())) //MulticastPlayNiagara(LockFX, true);
		{
			if (TargetPlayer->bKnocked)
			{
				ClearTarget();
			}
		}
	}
}

void ACPPTestCharacter::ThrowButtonReleased()
{
	if (combat && IsBallEquipped() && !bDoingAbility)
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
	combat->ThrowButtonPressed(true);

	if (combat && bEquipped)
	{
		MyThrow();
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
	bStunned = true;
	if (StunFX) {
		MulticastPlayNiagara(StunFX, true);
		ServerPlayNiagara(StunFX, true);
	}
	FTimerHandle StunHandle;
	GetWorld()->GetTimerManager().SetTimer(StunHandle, this, &ThisClass::StunCoolDown, StunDuration);
}

void ACPPTestCharacter::StunCoolDown()
{
	bStunned = false;
	if (StunFX) {
		MulticastPlayNiagara(StunFX, false);
		ServerPlayNiagara(StunFX, false);
	}
}

void ACPPTestCharacter::OnHealthUpdate()
{

	if (IsLocallyControlled())
	{


		if (CurrentHealth <= 0)
		{


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

			Knocked(ballHitDirection, bLeftGame);
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

void ACPPTestCharacter::Knocked(FVector ImpulseDirection, bool bPlayerLeftGame)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		MultiKnocked(ImpulseDirection, bPlayerLeftGame);

		if (IsBallEquipped())
		{
			combat->UnEquipBall(combat->equippedBall);
		}

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

bool ACPPTestCharacter::MultiKnocked_Validate(FVector ImpulseDirection, bool bPlayerLeftGame)
{
	return true;
}

void ACPPTestCharacter::MultiKnocked_Implementation(FVector ImpulseDirection, bool bPlayerLeftGame)
{


	bLeftGame = bPlayerLeftGame;
	bKnocked = true;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CharacterMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	CharacterMesh->SetAllBodiesSimulatePhysics(true);
	CharacterMesh->SetEnableGravity(false);
	CharacterMesh->SetAllMassScale(0.1);
	CharacterMesh->AddImpulse(GetActorLocation() + ((ImpulseDirection + GetActorUpVector()) * HitImpulse * CharacterMesh->GetMass()));

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

	if (bIsTargeting) {

		bIsTargeting = false;
		lockOnTargets->ClearTargetManual(false);

	}
}


void ACPPTestCharacter::DoEmote()
{
	if (ClappingAnim && !IsBallEquipped()) {
		ServerPlayAnimMontage(ClappingAnim, 1);
		MulticastPlayAnimMontage(ClappingAnim, 1);
	}
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
	if (CatchAnim)
	{
		PlayAnimMontage(CatchAnim, 1, NAME_None);
		ServerPlayAnimMontage(CatchAnim, 1, NAME_None);
	}

	bCatching = true;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACPPTestCharacter::CanCatch, CatchCooldown);

}

void ACPPTestCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACPPBall* BallHit = Cast<ACPPBall>(OtherActor))
	{
		if (BallHit->GetBallState() == EBallState::EBS_Initial && combat && !IsBallEquipped())
		{
			combat->EquipBall(BallHit);
		}
	}
}

void ACPPTestCharacter::MyThrow()
{
	if (combat && IsBallEquipped() && IsAllowedToMove() && bCanThrow)
	{
		combat->equippedBall->GetAreaSphere()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		if (combat->equippedBall->GetBallState() != EBallState::EBS_SuperThrow) {
			combat->equippedBall->SetBallState(EBallState::EBS_Dropped);
			combat->ThrowButtonPressed(false);
		}

		else if (combat->equippedBall->GetBallState() == EBallState::EBS_SuperThrow)
		{
			combat->equippedBall->AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			combat->equippedBall->AreaSphere->SetSimulatePhysics(false);
			combat->equippedBall->ProjectileMovementComponent->Activate(true);
			combat->equippedBall->ProjectileMovementComponent->bIsHomingProjectile = true;

		}
		bThrown = true;

		if (lockOnTargets->GetTarget())
		{
			combat->equippedBall->ProjectileMovementComponent->HomingTargetComponent = lockOnTargets->GetTarget()->GetRootComponent();
		}
		else
		{
			ClearTarget();
			FVector CameraLocation;
			FRotator CameraRotation;
			GetActorEyesViewPoint(CameraLocation, CameraRotation);
			combat->equippedBall->ProjectileMovementComponent->Velocity = CameraRotation.Vector() * throwPower;
		}
		bEquipped = false;
		combat->equippedBall->AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		combat->equippedBall = nullptr;
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
	if (HasAuthority())
		CPPPlayerController->SetHUDHealth(100.0F, MaxHealth);
}

void ACPPTestCharacter::OnRep_CurrentHealth()
{
	UpdateHUDHealth();
	OnHealthUpdate();

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

	ServerPlaySounds(Hit_Cue, GetActorLocation());

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
		MsgToRemove->RemoveFromParent();
	}
}

void ACPPTestCharacter::SetDashingAnimOff()
{
	bIsDashing = false;
	if (DashFX) {
		MulticastPlayNiagara(DashFX, false);
	}
}

//void ACPPTestCharacter::SpawnActors()
//{
//	//Find the Actor Spawner in the world, and invoke it's Spawn Actor function
//	AActor* ActorSpawnerTofind = UGameplayStatics::GetActorOfClass(GetWorld(), ABallSpawner::StaticClass());
//
//	ABallSpawner* ActorSpawnerReference = Cast<ABallSpawner>(ActorSpawnerTofind);
//	if (ActorSpawnerReference)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Found"));
//		ActorSpawnerReference->SpawnActor();
//	}
//}

void ACPPTestCharacter::PlaySounds(USoundCue* Cue, FVector Location)
{
	if (HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Cue, Location);
	}
	else
		ServerPlaySounds(Cue, Location);
}

void ACPPTestCharacter::MulticastPlaySounds_Implementation(USoundCue* Cue, FVector Location)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Cue, Location);
}

bool ACPPTestCharacter::MulticastPlaySounds_Validate(USoundCue* Cue, FVector Location)
{
	return true;
}

void ACPPTestCharacter::ServerPlaySounds_Implementation(USoundCue* Cue, FVector Location)
{
	MulticastPlaySounds(Cue, Location);
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
	}
	else
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



