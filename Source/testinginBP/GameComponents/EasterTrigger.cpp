// Fill out your copyright notice in the Description page of Project Settings.


#include "EasterTrigger.h"
#include "testinginBP/Character/CPPTestCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

// Sets default values
AEasterTrigger::AEasterTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//bReplicates = true;
	AreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	AreaBox->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEasterTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AreaBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
		AreaBox->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	}
	
}

// Called every frame
void AEasterTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEasterTrigger::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	//if(ACPPTestCharacter* player = Cast<ACPPTestCharacter>(otherActor))
	//{
	//	if (player->IsLocallyControlled())
	//	{
	//		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlay, GetActorLocation());
	//		//GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, TEXT("Al1"));
	//		//UGameplayStatics::GetPlayerController(GetWorld(), 1)->ClientPlaySoundAtLocation(SoundToPlay, GetActorLocation());

	//	}
	//	else
	//		//GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, TEXT("Alooooooooooooooo"));
	//		//UGameplayStatics::GetPlayerController(GetWorld(), 1)->ClientPlaySoundAtLocation(SoundToPlay, GetActorLocation());
	//	/*	
	//		int8 playerindex = player->GetController()->GetNetOwningPlayer()->GetPlayerController(GetWorld())->NetPlayerIndex;
	//	GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Blue, TEXT("Aloooooo2"));
	//	UGameplayStatics::GetPlayerController(GetWorld(), playerindex)->ClientPlaySoundAtLocation(SoundToPlay, GetActorLocation());*/
	//	}

	
}

void AEasterTrigger::OnSphereEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComponent, int32 otherBodyIndex)
{
}

void AEasterTrigger::Server_PlaySound_Implementation()
{
	


}

