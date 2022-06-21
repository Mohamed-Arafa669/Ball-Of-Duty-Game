// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPBall.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "testinginBP\Character\CPPTestCharacter.h"
#include "testinginBP/Character/StealCharacter.h"
#include "Net/UnrealNetwork.h" //Replication
#include "testinginBP\GameComponents\CombatComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"


ACPPBall::ACPPBall()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	RootComponent = AreaSphere;

	/*ballMesh = CreateDefaultSubobject <UStaticMeshComponent> (TEXT("Ball Mesh"));
	ballMesh->SetupAttachment(RootComponent);*/

	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	TrailFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailFX"));
	TrailFX->SetupAttachment(RootComponent);

	SuperTrailFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SuperTrailFX"));
	SuperTrailFX->SetupAttachment(RootComponent);

	SuperBallFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SuperBallFX"));
	SuperBallFX->SetupAttachment(RootComponent);

	BungeeGumFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Bungee Gum"));
	BungeeGumFX->SetupAttachment(RootComponent);


	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(AreaSphere);

}

void ACPPBall::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		AreaSphere->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}

	if (HasAuthority())
	{
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
	if (ACPPTestCharacter* testCharacter = Cast<ACPPTestCharacter>(otherActor))
	{
		if (testCharacter->bIsDashing && GetBallState() != EBallState::EBS_Stolen) {

			SetOwner(nullptr);
			ProjectileMovementComponent->HomingTargetComponent = nullptr;
			ProjectileMovementComponent->bIsHomingProjectile = false;
		}

	}
}

void ACPPBall::OnSphereEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex)
{
	
	if (ACPPTestCharacter* testCharacter = Cast<ACPPTestCharacter>(otherActor))
	{

		if (testCharacter->bIsDashing && GetBallState() != EBallState::EBS_Stolen) {

			SetOwner(nullptr);
			ProjectileMovementComponent->HomingTargetComponent = nullptr;
			ProjectileMovementComponent->bIsHomingProjectile = false;
		}
		/*if (testCharacter->IsBallEquipped()) {
			if (testCharacter->combat->equippedBall == GetOwner())
			{
				AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			}
		}*/
	}
}

void ACPPBall::OnRep_BallState()
{
	switch (ballState)
	{
	case EBallState::EBS_Equipped:

		ServerBallStateHandle(EBallState::EBS_Equipped);
		MulticastBallStateHandle(EBallState::EBS_Equipped);

		break;

	case EBallState::EBS_Dropped:

		ServerBallStateHandle(EBallState::EBS_Dropped);
		MulticastBallStateHandle(EBallState::EBS_Dropped);


		break;
	case EBallState::EBS_SuperThrow:

		ServerBallStateHandle(EBallState::EBS_SuperThrow);
		MulticastBallStateHandle(EBallState::EBS_SuperThrow);



	case EBallState::EBS_Stolen:

		ServerBallStateHandle(EBallState::EBS_Stolen);
		MulticastBallStateHandle(EBallState::EBS_Stolen);



	default:

		ServerBallStateHandle(EBallState::EBS_Initial);
		MulticastBallStateHandle(EBallState::EBS_Initial);

		break;
	}
}

void ACPPBall::SetBallState(EBallState state)
{
	ballState = state;
	switch (ballState)
	{
	case EBallState::EBS_Equipped:

		ServerBallStateHandle(EBallState::EBS_Equipped);
		MulticastBallStateHandle(EBallState::EBS_Equipped);
		break;

	case EBallState::EBS_Dropped:

		ServerBallStateHandle(EBallState::EBS_Dropped);
		MulticastBallStateHandle(EBallState::EBS_Dropped);
		break;
	case EBallState::EBS_SuperThrow:
		ServerBallStateHandle(EBallState::EBS_SuperThrow);
		MulticastBallStateHandle(EBallState::EBS_SuperThrow);
		break;
	case EBallState::EBS_Stolen:

		ServerBallStateHandle(EBallState::EBS_Stolen);
		MulticastBallStateHandle(EBallState::EBS_Stolen);

		break;
	default:

		ServerBallStateHandle(EBallState::EBS_Initial);
		MulticastBallStateHandle(EBallState::EBS_Initial);

		break;
	}

}

void ACPPBall::MoveHookedBall_Implementation(AStealCharacter* TargetPlayer)
{
	SetBallState(EBallState::EBS_Stolen);
	ProjectileMovementComponent->bIsHomingProjectile = true;
	ProjectileMovementComponent->HomingTargetComponent = TargetPlayer->GetRootComponent();
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
	if (ACPPTestCharacter* Player = Cast<ACPPTestCharacter>(OtherActor))
	{
		AController* ballOwnerController = this->GetInstigatorController();

		if (Player->bCatching && Player->combat)
		{
			if (Player->IsBallEquipped())
			{
				Player->combat->UnEquipBall(Player->combat->equippedBall);
			}
			Player->combat->EquipBall(this);
			Player->bCanThrow = true;
			Player->bSteal = false;
		}
		else if (Player->bIsSpawnInvincible)
		{
			ProjectileMovementComponent->bIsHomingProjectile = false;
			ProjectileMovementComponent->HomingTargetComponent = nullptr;
			if(AStealCharacter* tempCont = Cast<AStealCharacter>(ballOwnerController->GetCharacter()))
			{
				tempCont->bSteal = false;
				tempCont->isBungeeGum = false;
			} 
		}

		else if ((GetBallState() == EBallState::EBS_Dropped ||
			GetBallState() == EBallState::EBS_SuperThrow) &&
			Player->GetController() != ballOwnerController && !Player->bKnocked)
		{

			FDamageEvent GotHitEvent;

			Player->ballHitDirection = ballOwnerController->GetCharacter()->GetActorForwardVector();
			if (Player->HitEffect) {
				Player->MulticastPlayNiagara(Player->HitEffect, true);
				Player->ServerPlayNiagara(Player->HitEffect, true);
			}
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

			if (AStealCharacter* Steal = Cast<AStealCharacter>(ballOwnerController->GetCharacter()))
			{
				if (Steal->isBungeeGum)
				{
					SetOwner(nullptr);
					SetBallState(EBallState::EBS_Stolen);
					Steal->HomeOnMe();
				}
				else
				{
					SetBallState(EBallState::EBS_Initial);
					SetOwner(nullptr);
					SetInstigator(nullptr);
				}
			}
			else
			{

				SetBallState(EBallState::EBS_Initial);
				SetOwner(nullptr);
				SetInstigator(nullptr);
			}


		}
		else if (GetBallState() == EBallState::EBS_Stolen)
		{
			if (AStealCharacter* Steal = Cast<AStealCharacter>(Player)) {
				if (Steal->bSteal) {
					Steal->bSteal = false;
					if (Steal->IsBallEquipped())
					{
						Steal->combat->UnEquipBall(Steal->combat->equippedBall);
					}
					Steal->combat->EquipBall(this);
				}
				if (Steal->isBungeeGum) {
					if (Steal->IsBallEquipped())
					{
						Steal->combat->UnEquipBall(Steal->combat->equippedBall);
					}
					Steal->isBungeeGum = false;
					Steal->combat->EquipBall(this);
				}
			}

		}
		else if (GetBallState() == EBallState::EBS_Stolen && Player->GetController() != ballOwnerController)
		{
			AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		}

	}
	else
	{
		SetBallState(EBallState::EBS_Initial);
	}

}


void ACPPBall::MulticastBallStateHandle_Implementation(EBallState bs)
{
	if (bs == EBallState::EBS_Equipped)
	{
		ServerPlayNiagara(TrailFX, false);
		ServerPlayNiagara(SuperBallFX, false);
		ServerPlayNiagara(BungeeGumFX, false);
		ServerPlayNiagara(SuperTrailFX, false);

		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		
		ProjectileMovementComponent->HomingTargetComponent = nullptr;
		ProjectileMovementComponent->bIsHomingProjectile = false;
		ProjectileMovementComponent->Deactivate();
		

	}
	else if (bs == EBallState::EBS_Dropped)
	{
		ServerPlayNiagara(TrailFX, true);
		ServerPlayNiagara(SuperBallFX, false);
		ServerPlayNiagara(SuperTrailFX, false);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

		ProjectileMovementComponent->Activate(true);
		ProjectileMovementComponent->bIsHomingProjectile = true;
	}
	else if (bs == EBallState::EBS_SuperThrow)
	{

		ServerPlayNiagara(TrailFX, false);
		ServerPlayNiagara(SuperBallFX, true);
		ServerPlayNiagara(SuperTrailFX, true);
		

	}
	else if (bs == EBallState::EBS_Stolen)
	{

		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		ServerPlayNiagara(TrailFX, false);
		ServerPlayNiagara(BungeeGumFX, true);

		ProjectileMovementComponent->Activate(true);
		ProjectileMovementComponent->bIsHomingProjectile = true;


	}
	else if (bs == EBallState::EBS_Initial)
	{
		ServerPlayNiagara(TrailFX, false);
		ServerPlayNiagara(SuperBallFX, false);
		ServerPlayNiagara(SuperTrailFX, false);
		ServerPlayNiagara(BungeeGumFX, false);
		//ballMesh->SetRenderInDepthPass(false);

		//ballMesh->SetRenderInDepthPass(true);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		
		ProjectileMovementComponent->Activate(true);
		ProjectileMovementComponent->bIsHomingProjectile = false;
	}
}

bool ACPPBall::MulticastBallStateHandle_Validate(EBallState bs)
{
	return true;
}

void ACPPBall::ServerBallStateHandle_Implementation(EBallState bs)
{
	MulticastBallStateHandle(bs);
}

bool ACPPBall::ServerBallStateHandle_Validate(EBallState bs)
{
	return true;
}

