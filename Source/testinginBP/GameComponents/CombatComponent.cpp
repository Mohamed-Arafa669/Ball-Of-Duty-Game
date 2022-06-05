

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
				HUDPackager.crosshairCenter = equippedBall->crosshairsCenter;
				HUDPackager.crosshairLeft = equippedBall->crosshairsLeft;
				HUDPackager.crosshairRight = equippedBall->crosshairsRight;
				HUDPackager.crosshairBottom = equippedBall->crosshairsBottom;
				HUDPackager.crosshairTop = equippedBall->crosshairsTop;
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

	DOREPLIFETIME(UCombatComponent, equippedBall);
	//DOREPLIFETIME(UCombatComponent, )

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
		equippedBall->GetBallMesh()->AttachToComponent(character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("RightHandSocket"));
	}
	equippedBall->SetOwner(character);
	equippedBall->SetInstigator(character);
	
}

void UCombatComponent::UnEquipBall(class ACPPBall* BalltoUnequip)
{
	
	BalltoUnequip->GetBallMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	BalltoUnequip->SetBallState(EBallState::EBS_Dropped);
	BalltoUnequip->GetBallMesh()->SetSimulatePhysics(true);
	BalltoUnequip = nullptr;
	
}

