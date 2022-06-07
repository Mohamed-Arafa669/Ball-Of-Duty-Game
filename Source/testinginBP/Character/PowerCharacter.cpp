// Fill out your copyright notice in the Description page of Project Settings.

#include "PowerCharacter.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"
#include "CPPTestCharacter.h"
#include "DrawDebugHelpers.h"


APowerCharacter::APowerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;


}

void APowerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void APowerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Ability", IE_Pressed, this, &ThisClass::DoAbility);


}

void APowerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APowerCharacter, bSmash);

}


void APowerCharacter::BeginPlay()
{
	Super::BeginPlay();

	bSmash = false;
}



void APowerCharacter::AbilityCooldown()
{
	bSmash = false;
	OutHits.Empty();
}



void APowerCharacter::DoAbility()
{

	if (!bSmash) {

		if (HasAuthority()) {

			bSmash = true;
			FTimerHandle AbilityHandle;
			GetWorld()->GetTimerManager().SetTimer(AbilityHandle, this, &ThisClass::AbilityCooldown, Ability_Cooldown_Duration);


			FVector Start = GetActorLocation() + GetActorForwardVector() * StartDistance;
			FVector End = Start + (GetControlRotation().Vector() * EndDistance);

			GetWorld()->SweepMultiByChannel(OutHits, Start, End, FQuat::Identity, ECollisionChannel::ECC_Pawn,
				FCollisionShape::MakeSphere(RangeRadius));

			if(AbilityAnim)
			{
				MulticastPlayAnimMontage(AbilityAnim, 1, NAME_None);
			}

			if (OutHits.Num() > 0) {
				for (int i = 0; i < OutHits.Num(); i++)
				{

					DrawDebugSphere(GetWorld(), OutHits[i].GetActor()->GetActorLocation(), 100.f, 30, FColor::Red, false, 2.f);

					if (ACPPTestCharacter* Temp = Cast<ACPPTestCharacter>(OutHits[i].GetActor())) {


						if (GEngine)
						{
							GEngine->AddOnScreenDebugMessage(-1,
								5.f, FColor::Green, FString(Temp->GetName()));
						}

						Temp->Stunned();
					}

				}

			}

		}
		else
		{
			Server_DoAbility();
		}
	}
}



void APowerCharacter::Server_DoAbility_Implementation()
{
	bSmash = true;
	FTimerHandle AbilityHandle;
	GetWorld()->GetTimerManager().SetTimer(AbilityHandle, this, &ThisClass::AbilityCooldown, Ability_Cooldown_Duration);


	FVector Start = GetActorLocation() + GetActorForwardVector() * StartDistance;
	FVector End = Start + (GetControlRotation().Vector() * EndDistance);

	GetWorld()->SweepMultiByChannel(OutHits, Start, End, FQuat::Identity, ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(RangeRadius));

	if (AbilityAnim)
	{
		PlayAnimMontage(AbilityAnim, 1, NAME_None);
		MulticastPlayAnimMontage(AbilityAnim, 1, NAME_None);
	}

	if (OutHits.Num() > 0) {
		for (int i = 0; i < OutHits.Num(); i++)
		{

			DrawDebugSphere(GetWorld(), OutHits[i].GetActor()->GetActorLocation(), 100.f, 30, FColor::Red, false, 2.f);

			if (ACPPTestCharacter* Temp = Cast<ACPPTestCharacter>(OutHits[i].GetActor())) {


				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1,
						5.f, FColor::Green, FString(Temp->GetName()));
				}

				Temp->Stunned();
			}

		}

	}


}

