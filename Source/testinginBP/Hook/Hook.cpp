// Fill out your copyright notice in the Description page of Project Settings.


#include "Hook.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHook::AHook()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//Definition for the SphereComponent that will serve as the Root component for the projectile and its collision.
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->InitSphereRadius(37.5f);
	SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = SphereComponent;

	//Definition for the Mesh that will serve as our visual representation.
	//StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//StaticMesh->SetupAttachment(RootComponent);

	//Definition for the Projectile Movement Component.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
	ProjectileMovementComponent->InitialSpeed = 1500.0f;
	ProjectileMovementComponent->MaxSpeed = 1500.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);

	//Definition for the Projectile Particle System.
	//ExplosionEffect = CreateDefaultSubobject<UParticleSystem>(TEXT("ExplosionEffect"));

	//Effect = CreateDefaultSubobject<UNiagaraSystem>(TEXT("NiagaraSystem"));
	//Effect->ActivateSystem(false);

	// Delete the projectile after 3 seconds.
	/*InitialLifeSpan = 0.3f;*/
	this->InitialLifeSpan = 0.5f;

	///TODO Pull player by Impulse
	//USE CableComponent (Just Cosmetic)

	//Registering the Projectile Impact function on a Hit event.
	if (GetLocalRole() == ROLE_Authority)
	{
		SphereComponent->OnComponentHit.AddDynamic(this, &AHook::OnProjectileImpact);
	}

}

// Called when the game starts or when spawned
void AHook::BeginPlay()
{
	Super::BeginPlay();
	NiagaraComponent->Activate(true);
}

void AHook::Destroyed()
{
	FVector spawnLocation = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
	//Effect->ActivateSystem(true);
	//UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Effect, spawnLocation);
	NiagaraComponent->Deactivate();
}

void AHook::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Destrooooooy"));
	if (OtherActor)
	{
		/*UE_LOG(LogTemp, Warning, TEXT("Actor %s"), *OtherActor->GetName());
		StaticMesh->SetSimulatePhysics(true);
		StaticMesh->SetEnableGravity(false);
		StaticMesh->AddForce(GetActorLocation() - GetActorForwardVector() * 40000.0f);*/
		
	}

	//Destroy();
}

// Called every frame
void AHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

