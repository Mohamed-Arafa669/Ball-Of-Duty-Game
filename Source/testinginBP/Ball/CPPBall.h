// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "CPPBall.generated.h"

UENUM(BlueprintType)
enum class EBallState : uint8
{
	EBS_Initial	UMETA(DisplayName = "Initial State"),
	EBS_Equipped	UMETA(DisplayName = "Eqipped"),
	EBS_SuperThrow	UMETA(DisplayName = "Super Throw"),
	EBS_Stolen	UMETA(DisplayName = "Stolen"),
	EBS_Dropped	UMETA(DisplayName = "Dropped"),
	EBS_MAX	UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class TESTINGINBP_API ACPPBall : public AActor
{
	GENERATED_BODY()
	
public:	
	ACPPBall();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //Replication

	UPROPERTY(EditAnywhere, Category = "Ball Properties")
		UStaticMeshComponent* ballMesh;

	UPROPERTY(EditAnywhere, Category = "Ball Properties")
		class USphereComponent* AreaSphere;


	UPROPERTY(ReplicatedUsing = OnRep_BallState, VisibleAnywhere, Category = "Ball Properties")
		EBallState ballState;

	UPROPERTY(visibleAnywhere)
		class UCombatComponent* combat;


	// Projectile movement component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		class UProjectileMovementComponent* ProjectileMovementComponent;


protected:
	virtual void BeginPlay() override;


	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* overlappedComponent,
		AActor* otherActor,
		UPrimitiveComponent* otherComponent,
		int32 otherBodyIndex,
		bool bFromSweep,
		const FHitResult& sweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* overlappedComponent,
		AActor* otherActor,
		UPrimitiveComponent* otherComponent,
		int32 otherBodyIndex
	);

private:


	UFUNCTION()
	void OnRep_BallState();

	UPROPERTY(VisibleAnywhere, Category = "Ball Properties")
		class UWidgetComponent* pickUpWidget;


	
public:
	void SetBallState(EBallState state);
	EBallState GetBallState() const { return ballState; }
	void OnReleased();
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UStaticMeshComponent* GetBallMesh() const { return ballMesh; }

	UFUNCTION(NetMulticast, Reliable)
	void MoveHookedBall(class AStealCharacter* TargetPlayer);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MoveHooked")
		class AStealCharacter* Target;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* SuperTrailFX;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* TrailFX;
	UPROPERTY(EditAnywhere)
		UNiagaraComponent* BungeeGumFX;

	//UPROPERTY(EditAnywhere,BlueprintReadWrite)
	//URotatingMovementComponent* RotatingMovement;

protected:


	UPROPERTY(EditAnywhere)
		UNiagaraComponent* SuperBallFX;

	UFUNCTION(Server, Unreliable, WithValidation, Category = Effects)
		void ServerPlayNiagara(UNiagaraComponent* fx, bool state);

	UFUNCTION(NetMulticast, Unreliable, WithValidation, Category = Effects)
		void MulticastPlayNiagara(UNiagaraComponent* fx, bool state);
	

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(Server, Reliable, WithValidation, Category = BallHandling)
		void ServerBallStateHandle(EBallState bs);

	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = BallHandling)
		void MulticastBallStateHandle(EBallState bs);

};
