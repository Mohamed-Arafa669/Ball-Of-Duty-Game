// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CPPBall.generated.h"

UENUM(BlueprintType)
enum class EBallState : uint8
{
	EBS_Initial	UMETA(DisplayName = "Initial State"),
	EBS_Equipped	UMETA(DisplayName = "Eqipped"),
	EBS_Thrown	UMETA(DisplayName = "Thrown"),
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
	void ShowPickupWidget(bool bShowWidget);
	UPROPERTY(ReplicatedUsing = OnRep_BallState, VisibleAnywhere, Category = "Ball Properties")
		EBallState ballState;
	//Textures for the crosshair

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* crosshairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* crosshairsLeft;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* crosshairsRight;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* crosshairsTop;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* crosshairsBottom;

	UPROPERTY(visibleAnywhere)
		class UCombatComponent* combat;

	//////////////////////////////////////////////////////////////////////////////
	// ///////////////////////  Projectile Part  ////////////////////////////////
	// /////////////////////////////////////////////////////////////////////////
	// Sphere collision component.
	//UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	//	USphereComponent* CollisionComponent;

	//// Movement component for handling projectile movement.
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//	UProjectileMovementComponent* ProjectileMovementComponent;

	//// Function that initializes the projectile's velocity in the shoot direction.
	//void FireInDirection(const FVector& ShootDirection);
	// Sphere collision component

	/*UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		class USphereComponent* CollisionComponent;*/

	// Projectile movement component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		class UProjectileMovementComponent* ProjectileMovementComponent;

	//UFUNCTION(BlueprintImplementableEvent)
		void FindHomingTarget();

	/*UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void EventThrowing();*/

		UPROPERTY(visibleAnywhere)
		class UGameplayStatics* gameStatics;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<AActor> ClassToFind;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<AActor*> FoundActors;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<AActor*> OutActors;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<AActor*> CurrentHomingTargets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		AActor* FoundHomingTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector transform;

	double closestDistance = 1000000.0f;
/// <summary>
/// /////////////////////
/// </summary>

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
	UPROPERTY(VisibleAnywhere, Category = "Ball Properties")
		USkeletalMeshComponent* ballMesh;

	UPROPERTY(VisibleAnywhere, Category = "Ball Properties")
		class USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere, Category = "Ball Properties")
		class USceneComponent* ProjectileSceneComponent;
	

	UFUNCTION()
	void OnRep_BallState();

	UPROPERTY(VisibleAnywhere, Category = "Ball Properties")
		class UWidgetComponent* pickUpWidget;


	
public:
	void SetBallState(EBallState state);
	EBallState GetBallState() const { return ballState; }
	void OnReleased();
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetBallMesh() const { return ballMesh; }

	void MoveHookedBall(class AStealCharacter* TargetPlayer);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MoveHooked")
		class AStealCharacter* Target;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MoveHooked")
		float Speed = 500.0f;

	bool bMove;
	FVector Direction;
	float TotalDistance;
	float CurrentDistance;
	FVector StartLocation;
	static FVector Slerp(const FVector& a, const FVector& b, const float t)
	{
		float omega = FGenericPlatformMath::Acos(FVector::DotProduct(
			a.GetSafeNormal(),
			b.GetSafeNormal()
		));
		float sinOmega = FGenericPlatformMath::Sin(omega);
		FVector termOne = a * (FGenericPlatformMath::Sin(omega * (1.0 - t)) / sinOmega);
		FVector termTwo = b * (FGenericPlatformMath::Sin(omega * (t)) / sinOmega);
		return termOne + termTwo;
	}
	void mySlerp();
	void GetCenter(FVector& direction);

	UPROPERTY(EditAnywhere, Category = "ball")
		float startTime = 100.f;

	UPROPERTY(EditAnywhere, Category = "ball")
		float journeyTime = 1.f;

	UPROPERTY(EditAnywhere, Category = "ball")
		float ballSpeed = 10000.f;

	UPROPERTY(EditAnywhere, Category = "ball")
		FVector centerpoint;

	UPROPERTY(EditAnywhere, Category = "ball")
		FVector startRelCenter;

	UPROPERTY(EditAnywhere, Category = "ball")
		FVector endRelCenter;

	UPROPERTY(EditAnywhere, Category = "ball")
		FVector myTransform;

	UPROPERTY(visibleAnywhere)
		class UWorld* world;
};
