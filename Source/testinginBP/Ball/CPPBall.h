// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
};
