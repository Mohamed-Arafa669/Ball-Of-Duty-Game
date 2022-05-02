

#include "CombatComponent.h"
#include "testinginBP\Ball\CPPBall.h"
#include "testinginBP\Character\CPPTestCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h" //Replication


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


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
	eqippedBall->SetBallState(EBallState::EBS_Equipped);
	const USkeletalMeshSocket* handSocket = character->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (handSocket)
	{
		handSocket->AttachActor(eqippedBall, character->GetMesh());
	}
	eqippedBall->SetOwner(character);
	
}

