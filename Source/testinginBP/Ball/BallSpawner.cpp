// Fill out your copyright notice in the Description page of Project Settings.


#include "BallSpawner.h"
#include "Components/BoxComponent.h"
#include "testinginBP/Character/CPPTestCharacter.h"
#include "CPPBall.h"
#include "testinginBP/GameComponents/CombatComponent.h"

// Sets default values
ABallSpawner::ABallSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	SpawnVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	ShowBallFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailFX"));
	ShowBallFX->SetupAttachment(RootComponent);

	bCanSpawnBall = true;
}

ACPPBall* ABallSpawner::SpawnActor()
{
	bCanSpawnBall = false;
	if (ShowBallFX)
	{

		ServerPlayNiagara(ShowBallFX, false);
		MulticastPlayNiagara(ShowBallFX, false);
	}
	//ShowBallFX->Deactivate();

	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();
	RandIdx = FMath::RandRange(0, 1);
	SpawnedBall = GetWorld()->SpawnActor<ACPPBall>(Balls[RandIdx], SpawnLocation, SpawnRotation);
	SpawnedBall->SetBallState(EBallState::EBS_Initial);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &ThisClass::SpawnCoolDown, SpawnTimer);

	return SpawnedBall;
}

void ABallSpawner::SpawnCoolDown()
{
	bCanSpawnBall = true;
	if (ShowBallFX)
	{

		ServerPlayNiagara(ShowBallFX, true);
		MulticastPlayNiagara(ShowBallFX, true);
	}
	//ShowBallFX->Activate(true);
}

void ABallSpawner::ServerPlayNiagara_Implementation(UNiagaraComponent* fx, bool state)
{
	MulticastPlayNiagara(fx, state);
}

void ABallSpawner::MulticastPlayNiagara_Implementation(UNiagaraComponent* fx, bool state)
{
	if (!state)
	{
		fx->DeactivateImmediate();
		//fx->EndPlay(EEndPlayReason::RemovedFromWorld);
		UE_LOG(LogTemp, Warning, TEXT("Deactivate"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Activate"));
		fx->Activate(state);
	}
}

bool ABallSpawner::MulticastPlayNiagara_Validate(UNiagaraComponent* fx, bool state)
{
	return true;
}

// Called when the game starts or when spawned
void ABallSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		SpawnVolume->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
		//ShowBallFX->Activate(true);
	}

	ServerPlayNiagara(ShowBallFX, true);
	MulticastPlayNiagara(ShowBallFX, true);
}

// Called every frame
void ABallSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABallSpawner::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (bCanSpawnBall)
	{
		
		if (ACPPTestCharacter* Player = Cast<ACPPTestCharacter>(OtherActor))
		{
			

			if (!Player->IsBallEquipped())
			{
				Player->combat->EquipBall(SpawnActor());
			}
		}
	}
}

