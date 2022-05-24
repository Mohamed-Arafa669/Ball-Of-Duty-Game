// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPBall.h"
#include "Components/MeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "testinginBP\Character\CPPTestCharacter.h"
#include "testinginBP/Character/StealCharacter.h"
#include "Net/UnrealNetwork.h" //Replication
#include "testinginBP\GameComponents\CombatComponent.h"

ACPPBall::ACPPBall()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//SetBallState(EBallState::EBS_Dropped);

	ballMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BallMesh"));
	ballMesh->SetupAttachment(RootComponent);
	SetRootComponent(ballMesh);

	ballMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);							 //to bounce when dropped
	ballMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Block); //pawn ignore for not colliding
	ballMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	////Definition for the Projectile Movement Component.
	//ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	//ProjectileMovementComponent->SetUpdatedComponent(AreaSphere);
	//ProjectileMovementComponent->InitialSpeed = 1500.0f;
	//ProjectileMovementComponent->MaxSpeed = 1500.0f;
	//ProjectileMovementComponent->bRotationFollowsVelocity = true;
	//ProjectileMovementComponent->ProjectileGravityScale = 1.0f;
	//ProjectileMovementComponent->bIsHomingProjectile = false; //To be set later

	pickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	pickUpWidget->SetupAttachment(RootComponent);

	bMove = false;
}

void ACPPBall::BeginPlay()
{
	Super::BeginPlay();
	if (pickUpWidget)
	{
		pickUpWidget->SetVisibility(false);
	}
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ACPPBall::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ACPPBall::OnSphereEndOverlap);

	}
}

void ACPPBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bMove)
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
	}

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
		break;

	case EBallState::EBS_Dropped:
		ShowPickupWidget(true);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	default:
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
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EBallState::EBS_Dropped:
		ShowPickupWidget(true);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	default:
		break;
	}
	
}

void ACPPBall::OnReleased()
{
	//combat->eqippedBall->SetBallState(EBallState::EBS_Dropped);
	//ballMesh->DetachFromParent(true);
	//if(ballMesh == nullptr) return

	//ballMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	
	//combat->eqippedBall->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	//combat->eqippedBall->DetachRootComponentFromParent();
	//combat->eqippedBall -> ballMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	//combat->eqippedBall->ballMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//ballMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	//ballMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//	ballMesh->SetSimulatePhysics(true);
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

void ACPPBall::ShowPickupWidget(bool bShowWidget)
{
	if (pickUpWidget)
	{
		pickUpWidget->SetVisibility(bShowWidget);
	}
}

