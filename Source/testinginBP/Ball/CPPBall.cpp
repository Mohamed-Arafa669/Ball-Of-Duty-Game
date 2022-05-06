// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPBall.h"
#include "Components/MeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "testinginBP\Character\CPPTestCharacter.h"
#include "Net/UnrealNetwork.h" //Replication
#include "testinginBP\GameComponents\CombatComponent.h"

ACPPBall::ACPPBall()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ballMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BallMesh"));
	ballMesh->SetupAttachment(RootComponent);
	SetRootComponent(ballMesh);

	ballMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);							 //to bounce when dropped
	ballMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Ignore); //pawn ignore for not colliding
	ballMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	pickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	pickUpWidget->SetupAttachment(RootComponent);
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

void ACPPBall::ShowPickupWidget(bool bShowWidget)
{
	if (pickUpWidget)
	{
		pickUpWidget->SetVisibility(bShowWidget);
	}
}

