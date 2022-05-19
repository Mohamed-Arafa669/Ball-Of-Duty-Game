// Fill out your copyright notice in the Description page of Project Settings.

#include "PowerCharacter.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"
#include "CPPTestCharacter.h"


APowerCharacter::APowerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HitAreaCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HitAreaCone"));
	HitAreaCone->SetupAttachment(GetMesh());
}

void APowerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority()) {
		//HitAreaCone->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::PowerAbilityOnOverlapBegin);

		HitAreaCone->GetOverlappingActors(OverlappingActors);
	}
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
	bEffectOn = false;


}


//void APowerCharacter::PowerAbilityOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
//	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (bSmash) {
//
//		if (ACPPTestCharacter* Enemy = Cast<ACPPTestCharacter>(OtherActor))
//		{
//			Enemy->bCanMove = false;
//
//			FTimerHandle handle;
//			FTimerDelegate StunDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::StunCooldown, Enemy);
//
//			GetWorld()->GetTimerManager().SetTimer(handle, StunDelegate, 2.f, false);
//		}
//	}
//}


void APowerCharacter::AbilityCooldown()
{
	bSmash = false;
}



void APowerCharacter::EffectCooldown()
{
	bEffectOn = false;
	HitAreaCone->SetVisibility(false);

}


void APowerCharacter::DoAbility()
{

	if (!bSmash) {
		if (HasAuthority()) {
			HitAreaCone->SetVisibility(true);
			bSmash = true;
			FTimerHandle AbilityHandle;
			GetWorld()->GetTimerManager().SetTimer(AbilityHandle, this, &ThisClass::AbilityCooldown, 1.f);

			bEffectOn = true;
			FTimerHandle EffectHandle;
			GetWorld()->GetTimerManager().SetTimer(EffectHandle, this, &ThisClass::EffectCooldown, 1.f);

			if (bEffectOn) {

				HitAreaCone->SetVisibility(true);
				if (OverlappingActors.Num() > 0) {
					for (int i = 0; i < OverlappingActors.Num(); i++)
					{
						if (OverlappingEnemies.Add(Cast<ACPPTestCharacter>(OverlappingActors[i]))) {

							if (GEngine)
							{
								GEngine->AddOnScreenDebugMessage(-1,
									5.f, FColor::Green, FString(OverlappingEnemies[i]->GetName()));
							}

							OverlappingEnemies[i]->Stunned();
						}
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
	HitAreaCone->SetVisibility(true);
	bSmash = true;
	FTimerHandle AbilityHandle;
	GetWorld()->GetTimerManager().SetTimer(AbilityHandle, this, &ThisClass::AbilityCooldown, 1.f);

	bEffectOn = true;
	FTimerHandle EffectHandle;
	GetWorld()->GetTimerManager().SetTimer(EffectHandle, this, &ThisClass::EffectCooldown, 1.f);

	if (bEffectOn) {

		HitAreaCone->SetVisibility(true);
		if (OverlappingActors.Num() > 0) {
			for (int i = 0; i < OverlappingActors.Num(); i++)
			{
				if (OverlappingEnemies.Add(Cast<ACPPTestCharacter>(OverlappingActors[i]))) {

					if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(-1,
							5.f, FColor::Green, FString(OverlappingEnemies[i]->GetName()));
					}

					OverlappingEnemies[i]->Stunned();
				}
			}
		}
	}

}

