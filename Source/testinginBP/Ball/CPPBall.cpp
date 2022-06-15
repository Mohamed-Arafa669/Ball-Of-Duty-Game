// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPBall.h"
#include "Components/MeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "testinginBP\Character\CPPTestCharacter.h"
#include "testinginBP/Character/StealCharacter.h"
#include "Net/UnrealNetwork.h" //Replication
#include "testinginBP\GameComponents\CombatComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../LockOnTarget/Source/LockOnTarget/Public/LockOnTargetComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"


ACPPBall::ACPPBall()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	
	//SetBallState(EBallState::EBS_Dropped);
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	RootComponent = AreaSphere;

	/*ballMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	ballMesh->SetupAttachment(RootComponent);*/


	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	TrailFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailFX"));
	TrailFX->SetupAttachment(RootComponent);

	SuperBallFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SuperBallFX"));
	SuperBallFX->SetupAttachment(RootComponent);

	
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(AreaSphere);
		/*ProjectileMovementComponent->InitialSpeed = 3000.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;*/
		//ProjectileMovementComponent->Activate(true);
		
		
		

	pickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	pickUpWidget->SetupAttachment(RootComponent);

	bMove = false;

}

void ACPPBall::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		AreaSphere->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
		//ProjectileMovementComponent->Activate(true);
	}
	

	if (pickUpWidget)
	{
		pickUpWidget->SetVisibility(false);
	}
	if (HasAuthority())
	{
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ACPPBall::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ACPPBall::OnSphereEndOverlap);

		//DisableComponentsSimulatePhysics();

	}
	//SetBallState(EBallState::EBS_Initial);
}

void ACPPBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (bMove)
	{
		UE_LOG(LogTemp, Warning, TEXT("Moving"));

		if (Target != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Movinsssg"));

			if (CurrentDistance < TotalDistance)
			{
				FVector Location = GetActorLocation();
				Location += Direction * Speed * DeltaTime;
				SetActorLocation(Location);
				CurrentDistance = (Location - StartLocation).Size();
				UE_LOG(LogTemp, Warning, TEXT("cURRENT DIS %f"), CurrentDistance);
			}
			else
				bMove = false;
		}
	}*/

}

void ACPPBall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACPPBall, ballState);
}

void ACPPBall::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	ACPPTestCharacter* testCharacter = Cast<ACPPTestCharacter>(otherActor);
	if (testCharacter && pickUpWidget)
	{
		testCharacter->SetOverlappingBall(this);
	} else
	{
		//SetBallState(EBallState::EBS_Initial);
	}
	
}



void ACPPBall::OnSphereEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex)
{
	ACPPTestCharacter* testCharacter = Cast<ACPPTestCharacter>(otherActor);
	if (testCharacter && pickUpWidget)
	{
		testCharacter->SetOverlappingBall(nullptr);
	}
}

void ACPPBall::OnRep_BallState()
{
	switch (ballState)
	{
	case EBallState::EBS_Equipped:
		ShowPickupWidget(false);
		ServerPlayNiagara(TrailFX, false);
		ServerPlayNiagara(SuperBallFX, false);
		MulticastPlayNiagara_Implementation(TrailFX, false);
		MulticastPlayNiagara_Implementation(SuperBallFX, false);
		AreaSphere->SetSimulatePhysics(false);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		ProjectileMovementComponent->Deactivate();
		ProjectileMovementComponent->bIsHomingProjectile = false;
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		break;

	case EBallState::EBS_Dropped:
		ShowPickupWidget(true);
		ServerPlayNiagara(TrailFX, true);
		MulticastPlayNiagara_Implementation(TrailFX, true);
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		AreaSphere->SetSimulatePhysics(false);
		ProjectileMovementComponent->Activate(true);
		ProjectileMovementComponent->bIsHomingProjectile = true;
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EBallState::EBS_SuperThrow:
		MulticastPlayNiagara_Implementation(TrailFX, true);
		ServerPlayNiagara(SuperBallFX, true);
		ServerPlayNiagara(TrailFX, true);
		MulticastPlayNiagara_Implementation(SuperBallFX, true);
		AreaSphere->SetSimulatePhysics(false);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		ProjectileMovementComponent->Activate(true);
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ProjectileMovementComponent->bIsHomingProjectile = true;

	case EBallState::EBS_SimPhy:
		MulticastPlayNiagara_Implementation(TrailFX, true);
		ServerPlayNiagara(SuperBallFX, false);
		ServerPlayNiagara(TrailFX, true);
		MulticastPlayNiagara_Implementation(SuperBallFX, false);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		AreaSphere->SetSimulatePhysics(true);
		ProjectileMovementComponent->Deactivate();
		ProjectileMovementComponent->bIsHomingProjectile = false;

	default:
		ServerPlayNiagara(TrailFX, false);
		ServerPlayNiagara(SuperBallFX, false);
		MulticastPlayNiagara_Implementation(TrailFX, false);
		MulticastPlayNiagara_Implementation(SuperBallFX, false);
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AreaSphere->SetSimulatePhysics(true);
		ProjectileMovementComponent->Deactivate();
		ProjectileMovementComponent->bIsHomingProjectile = false;
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		break;
	}
}

void ACPPBall::SetBallState(EBallState state)
{
	ballState = state;
	switch (ballState)
	{
	case EBallState::EBS_Equipped:
		ShowPickupWidget(false);
		ServerPlayNiagara(TrailFX, false);
		ServerPlayNiagara(SuperBallFX, false);
		MulticastPlayNiagara_Implementation(TrailFX, false);
		MulticastPlayNiagara_Implementation(SuperBallFX, false);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		AreaSphere->SetSimulatePhysics(false);
		ProjectileMovementComponent->Deactivate();
		ProjectileMovementComponent->bIsHomingProjectile = false;
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EBallState::EBS_Dropped:
		ShowPickupWidget(true);
		ServerPlayNiagara(TrailFX, true);
		MulticastPlayNiagara_Implementation(TrailFX, true);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		AreaSphere->SetSimulatePhysics(false);
		ProjectileMovementComponent->Activate(true);
		ProjectileMovementComponent->bIsHomingProjectile = true;
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EBallState::EBS_SuperThrow:
		MulticastPlayNiagara_Implementation(TrailFX, true);
		ServerPlayNiagara(SuperBallFX, true);
		ServerPlayNiagara(TrailFX, true);
		MulticastPlayNiagara_Implementation(SuperBallFX, true);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		AreaSphere->SetSimulatePhysics(false);
		ProjectileMovementComponent->Activate(true);
		ProjectileMovementComponent->bIsHomingProjectile = true;
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EBallState::EBS_SimPhy:
		MulticastPlayNiagara_Implementation(TrailFX, true);
		ServerPlayNiagara(SuperBallFX, false);
		ServerPlayNiagara(TrailFX, true);
		MulticastPlayNiagara_Implementation(SuperBallFX, false);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		AreaSphere->SetSimulatePhysics(true);
		ProjectileMovementComponent->Deactivate();
		ProjectileMovementComponent->bIsHomingProjectile = false;
		break;
	default:
		ServerPlayNiagara(TrailFX, false);
		ServerPlayNiagara(SuperBallFX, false);
		MulticastPlayNiagara_Implementation(TrailFX, false);
		MulticastPlayNiagara_Implementation(SuperBallFX, false);
		AreaSphere->SetSimulatePhysics(true);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		ProjectileMovementComponent->Deactivate();
		ProjectileMovementComponent->bIsHomingProjectile = false;


		break;
	}
	
}

void ACPPBall::OnReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("???"));
}

void ACPPBall::MoveHookedBall(class AStealCharacter* TargetPlayer)
{
	Target = TargetPlayer;
	if (Target != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bla bla bla"));
		StartLocation = GetActorLocation();
		Direction = Target->GetActorLocation() - StartLocation;
		TotalDistance = Direction.Size();

		UE_LOG(LogTemp, Warning, TEXT("Distance = %f"), TotalDistance);

		Direction = Direction.GetSafeNormal();
		CurrentDistance = 0.0f;

		bMove = true;

	}
}

void ACPPBall::ServerPlayNiagara_Implementation(UNiagaraComponent* fx, bool state)
{
	MulticastPlayNiagara(fx, state);
}

bool ACPPBall::ServerPlayNiagara_Validate(UNiagaraComponent* fx, bool state)
{
	return true;
}

void ACPPBall::MulticastPlayNiagara_Implementation(UNiagaraComponent* fx, bool state)
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

bool ACPPBall::MulticastPlayNiagara_Validate(UNiagaraComponent* fx, bool state)
{
	return true;
}

void ACPPBall::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{

	/*FString msg1 = OtherActor->GetName();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, *msg1);*/

	if (ACPPTestCharacter* Player = Cast<ACPPTestCharacter>(OtherActor))
	{
		AController* ballOwnerController = this->GetInstigatorController();

		if (Player->bCatching && Player->combat && !Player->IsBallEquipped() || Player->bSteal)
		{
			Player->combat->EquipBall(this);
			Player->bCanThrow = true;
			Player->bSteal = false;
		}

		else if ((GetBallState() == EBallState::EBS_Dropped ||
			GetBallState() == EBallState::EBS_SuperThrow) && !Player->bIsSpawnInvincible &&
			Player->GetController() != ballOwnerController && !Player->bKnocked) 
		{

			FDamageEvent GotHitEvent;

			Player->ballHitDirection = ballOwnerController->GetCharacter()->GetActorForwardVector();

			if (GetBallState() == EBallState::EBS_SuperThrow)
			{
				Player->Knocked(Player->ballHitDirection, false);
				UGameplayStatics::ApplyDamage(Player, 100.f, ballOwnerController, this, NULL);

			}
			else
			{
				UGameplayStatics::ApplyDamage(Player, 50.f, ballOwnerController, this, NULL);
			}

			if (Player->CurrentHealth > 0 && Player->GetHitAnim) {
				Player->MulticastPlayAnimMontage(Player->GetHitAnim, 1, NAME_None);
			}

			//this->TakeDamage(50.f, GotHitEvent, BallHit->GetInstigatorController(), BallHit);
			//TODO : Make A reset function for owner and instigator
			SetBallState(EBallState::EBS_SimPhy);
			SetOwner(nullptr);
			SetInstigator(nullptr);
		}
		else if (GetBallState() == EBallState::EBS_Initial && Player->combat && !Player->IsBallEquipped())
		{
			Player->combat->EquipBall(this);
		}
	}
	else
	{
		SetBallState(EBallState::EBS_Initial);
	}

}

//void ACPPBall::mySlerp()
//{
//	
//	SetActorLocation(Slerp(startRelCenter, endRelCenter, ballSpeed), false, (FHitResult*) nullptr, ETeleportType::ResetPhysics);
//	//SetActorLocationAndRotation(,GetActorRotation());
//	 ;
//	//myTransform += centerpoint;
//}

void ACPPBall::ShowPickupWidget(bool bShowWidget)
{
	if (pickUpWidget)
	{
		pickUpWidget->SetVisibility(bShowWidget);
	}
}

void ACPPBall::FindHomingTarget()
{
		
}
