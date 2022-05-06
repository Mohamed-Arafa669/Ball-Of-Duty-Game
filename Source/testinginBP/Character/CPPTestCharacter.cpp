

#include "CPPTestCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h" //Replication
#include "testinginBP\Ball\CPPBall.h"
#include "testinginBP\GameComponents\CombatComponent.h"
#include "Animation/AnimMontage.h"
#include "testinginBP\Character\CPPAnimInstance.h"

#include "Kismet//KismetMathLibrary.h"

ACPPTestCharacter::ACPPTestCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	cameraBoom->SetupAttachment(GetMesh());
	cameraBoom->TargetArmLength = 300.0f;
	cameraBoom->bUsePawnControlRotation = true;

	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);
	followCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	overHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	overHeadWidget->SetupAttachment(RootComponent);

	combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	combat->SetIsReplicated(true);

	bCanDash = true;



}
void ACPPTestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//important to include #include "Net/UnrealNetwork.h" whenever we use the replication macro
	DOREPLIFETIME_CONDITION(ACPPTestCharacter, overlappingBall, COND_OwnerOnly); //Replication
}


void ACPPTestCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (combat)
	{
		combat->character = this;
	}
}

void ACPPTestCharacter::PlayThrowMontage()
{
	if (combat == nullptr || combat->eqippedBall == nullptr) return;

	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && throwAnim)
	{
		animInstance->Montage_Play(throwAnim);
		ServerPlayAnimMontage(throwAnim);
		//animInstance->OnPlayMontageNotifyBegin;
		
		
	}
}

void ACPPTestCharacter::BeginPlay()
{
	Super::BeginPlay();

}
void ACPPTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void ACPPTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ACPPTestCharacter::EquipButtonPressed);

	//PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &ACPPTestCharacter::Throw);

	PlayerInputComponent->BindAction("Catch", IE_Pressed, this, &ACPPTestCharacter::Catch);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ACPPTestCharacter::Dash);

	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &ACPPTestCharacter::ThrowButtonPressed);
	PlayerInputComponent->BindAction("Throw", IE_Released, this, &ACPPTestCharacter::ThrowButtonReleased);



	PlayerInputComponent->BindAxis("MoveForward", this, &ACPPTestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACPPTestCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ACPPTestCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ACPPTestCharacter::LookUp);

}

void ACPPTestCharacter::MoveForward(float value)
{
	if (Controller != nullptr && value != 0.0f)
	{
		const FRotator yawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(direction, value);
	}
}

void ACPPTestCharacter::MoveRight(float value)
{
	if (Controller != nullptr && value != 0.0f)
	{
		const FRotator yawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(direction, value);
	}
}

void ACPPTestCharacter::Turn(float value)
{
	AddControllerYawInput(value);
}

void ACPPTestCharacter::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void ACPPTestCharacter::EquipButtonPressed()
{
	if (combat)
	{
		if (HasAuthority())
		{
			combat->EquipBall(overlappingBall);

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
		combat->EquipBall(overlappingBall);
	}
}

void ACPPTestCharacter::Catch()
{
	if(combat && IsBallEquipped() == true)
	{
		if (HasAuthority() )
		{
			UKismetMathLibrary::GetForwardVector(GetControlRotation()) *= 25000.0f;

			combat->eqippedBall->SetBallState(EBallState::EBS_Initial);
			combat->ThrowButtonPressed(false); //gded
			combat->eqippedBall->GetBallMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			//combat->eqippedBall->DetachRootComponentFromParent(true);
			SetOverlappingBall(combat->eqippedBall);
			OnRep_OverlappingBall(combat->eqippedBall);
			combat->eqippedBall->GetBallMesh()->SetSimulatePhysics(true);

			combat->eqippedBall->GetBallMesh()->SetPhysicsLinearVelocity(FVector::ForwardVector);
			combat->eqippedBall->GetBallMesh()->AddImpulse(UKismetMathLibrary::GetForwardVector(GetControlRotation()));
		}
		else
		{
			ServerCatch();
		}
		
	}
}

void ACPPTestCharacter::ServerCatch_Implementation()
{
	if (combat && IsBallEquipped() == true)
	{
		UKismetMathLibrary::GetForwardVector(GetControlRotation()) *= 25000.0f;

		combat->eqippedBall->SetBallState(EBallState::EBS_Initial);
		combat->ThrowButtonPressed(false); //gded
		combat->eqippedBall->GetBallMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		//combat->eqippedBall->DetachRootComponentFromParent(true);

		SetOverlappingBall(combat->eqippedBall);
		OnRep_OverlappingBall(combat->eqippedBall);
		combat->eqippedBall->GetBallMesh()->SetSimulatePhysics(true);

		combat->eqippedBall->GetBallMesh()->SetPhysicsLinearVelocity(FVector::ForwardVector);
		combat->eqippedBall->GetBallMesh()->AddImpulse(UKismetMathLibrary::GetForwardVector(GetControlRotation()));

	}
}

void ACPPTestCharacter::Dash()
{

	if (HasAuthority())
	{
		if (bCanDash) {



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

void ACPPTestCharacter::DashButtonPressed_Implementation()
{
	if (bCanDash) {

		if (DashAnim)
		{
			PlayAnimMontage(DashAnim, 1, NAME_None);
			ServerPlayAnimMontage(DashAnim, 1, NAME_None);

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Red,
					FString::Printf(TEXT("ALO")));
			}
		}

		const FVector ForwardVector = this->GetActorRotation().Vector();
		LaunchCharacter(ForwardVector * DashDistance, true, true);

		bCanDash = false;

		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &ThisClass::CanDash, 1.f);
	}
}

void ACPPTestCharacter::ThrowButtonPressed()
{
	if(combat)
	{
		if (HasAuthority())
		{
			combat->ThrowButtonPressed(true);
			//OnBallReleased();
		}
		else
		{
			ServerThrowButtonPressed();
		}
	}
	
}
void ACPPTestCharacter::ServerThrowButtonPressed_Implementation()
{
		if (combat)
		{
			combat->ThrowButtonPressed(true);
		}

}

void ACPPTestCharacter::ThrowButtonReleased()
{
	//if (overlappingBall)
	//{
	//	//combat->ThrowButtonPressed(false);
	//	overlappingBall->DetachFromActor
	//	
	//}

}


void ACPPTestCharacter::OnBallReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("md5lsh"));
	if (combat)
	{
		UE_LOG(LogTemp, Warning, TEXT("d5l ?"));
		overlappingBall->OnReleased();
		//TODO
		// 1- Stop Velocity
		// 2- Add impulse
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
			overlappingBall->DisableComponentsSimulatePhysics();
		}
	}
}

void ACPPTestCharacter::OnRep_OverlappingBall(ACPPBall* lastBall)
{
	if (overlappingBall)
	{
		overlappingBall->ShowPickupWidget(true);
		overlappingBall->DisableComponentsSimulatePhysics();
	}
	if (lastBall)
	{
		lastBall->ShowPickupWidget(false);
	}
}



bool ACPPTestCharacter::IsBallEquipped()
{
	return(combat && combat->eqippedBall);

	if (combat && combat->eqippedBall)
	{
		return true;
	}
	else
	{
		return false;
	}
}






