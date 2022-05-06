

#include "CombatComponent.h"
#include "testinginBP\Ball\CPPBall.h"
#include "testinginBP\Character\CPPTestCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h" //Replication
#include "testinginBP\PlayerController\CPPPlayerController.h"
#include "testinginBP\HUD\GameHUD.h"
#include "Animation/AnimMontage.h"


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
		character->PlayThrowMontage();
	}
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SetHUDCrosshairs(DeltaTime);

}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (character == nullptr || character->Controller == nullptr) return;

	Controller = Controller == nullptr ? Cast<ACPPPlayerController>(character->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<AGameHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			FHUDPackage HUDPackager;

			/*if (eqippedBall)
			{*/
				HUDPackager.crosshairCenter = eqippedBall->crosshairsCenter;
				HUDPackager.crosshairLeft = eqippedBall->crosshairsLeft;
				HUDPackager.crosshairRight = eqippedBall->crosshairsRight;
				HUDPackager.crosshairBottom = eqippedBall->crosshairsBottom;
				HUDPackager.crosshairTop = eqippedBall->crosshairsTop;
				/*}
				else
				{
					HUDPackager.crosshairCenter = nullptr;
					HUDPackager.crosshairLeft = nullptr;
					HUDPackager.crosshairRight = nullptr;
					HUDPackager.crosshairBottom = nullptr;
					HUDPackager.crosshairTop = nullptr;
				}*/
			HUD->SetHUDPackage(HUDPackager);


		}
	}
}



void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, eqippedBall);

}

void UCombatComponent::EquipBall(class ACPPBall* ballToEquip)
{
	if (character == nullptr || ballToEquip == nullptr) return;

	eqippedBall = ballToEquip;
	eqippedBall->DisableComponentsSimulatePhysics();
	eqippedBall->SetBallState(EBallState::EBS_Equipped);
	handSocket = character->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (handSocket)
	{
		eqippedBall->GetBallMesh()->AttachToComponent(character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("RightHandSocket"));
	}
	eqippedBall->SetOwner(character);
	
}

void UCombatComponent::UnEquipBall(ACPPBall* ballToEquip)
{
	if (character == nullptr || ballToEquip == nullptr) return;

	eqippedBall = ballToEquip;
	//eqippedBall->SetBallState(EBallState::EBS_Initial);
	handSocket = character->GetMesh()->GetSocketByName(FName("RightHandSocket"));


	if (handSocket)
	{
		eqippedBall->GetBallMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		eqippedBall->GetBallMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		eqippedBall->GetBallMesh()->SetSimulatePhysics(true);
	}
	eqippedBall->SetOwner(character);
}

