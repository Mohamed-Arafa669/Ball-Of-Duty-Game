

#include "CombatComponent.h"
#include "testinginBP\Ball\CPPBall.h"
#include "testinginBP\Character\CPPTestCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h" //Replication
#include "testinginBP\PlayerController\CPPPlayerController.h"
#include "testinginBP\HUD\GameHUD.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetMathLibrary.h"


UCombatComponent::UCombatComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UCombatComponent::ThrowButtonPressed(bool bPressed)
{
	bThrowButtonPressed = bPressed;
	
	if (character)
	{
		//character->bEquipped = false;
		character->PlayThrowMontage();
	}
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	
}



void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, equippedBall);


}

void UCombatComponent::EquipBall(class ACPPBall* ballToEquip)
{
	if (character == nullptr || ballToEquip == nullptr) return;

	character->bEquipped = true;
	equippedBall = ballToEquip;
	equippedBall->DisableComponentsSimulatePhysics();
	equippedBall->SetBallState(EBallState::EBS_Equipped);
	handSocket = character->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (handSocket)
	{
		//equippedBall->GetBallMesh()->AttachToComponent(character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("RightHandSocket"));
		equippedBall->GetAreaSphere()->AttachToComponent(character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("RightHandSocket"));
		
	}
	equippedBall->SetOwner(character);
	equippedBall->SetInstigator(character);

	equippedBall->AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	//character->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	equippedBall->AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	equippedBall->AreaSphere->SetSimulatePhysics(false);
	equippedBall->ProjectileMovementComponent->bIsHomingProjectile = false;
	equippedBall->ProjectileMovementComponent->Deactivate();

	
}

void UCombatComponent::UnEquipBall(class ACPPBall* BalltoUnequip)
{
	character->bEquipped = false;
	if (BalltoUnequip) {
		BalltoUnequip->GetAreaSphere()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		BalltoUnequip->SetBallState(EBallState::EBS_Initial);
		//BalltoUnequip->GetAreaSphere()->SetSimulatePhysics(true);
		BalltoUnequip = nullptr;
	}
	
}

