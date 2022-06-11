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



	if (!ProjectileMovementComponent)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		//ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 0.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

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
	}
	

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
	//SetBallState(EBallState::EBS_Initial);
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
	} else
	{
		SetBallState(EBallState::EBS_Initial);
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
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EBallState::EBS_Dropped:
		ShowPickupWidget(true);
		//AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	default:
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

void ACPPBall::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(ACPPTestCharacter* Player = Cast<ACPPTestCharacter>(OtherActor))
	{
		return;
	} else
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
