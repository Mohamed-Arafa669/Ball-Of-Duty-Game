// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTINGINBP_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ACPPTestCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //Replication

	void EquipBall(class ACPPBall* ballToEquip);
	void UnEquipBall(ACPPBall* ballToEquip);
	UPROPERTY(Replicated)
		ACPPBall* equippedBall;
	UPROPERTY(visibleAnywhere)
	 const USkeletalMeshSocket* handSocket;

	UPROPERTY()
	class ACPPTestCharacter* character;

protected:
	virtual void BeginPlay() override;

	

	void SetHUDCrosshairs(float DeltaTime);

private:

	UPROPERTY()
	class ACPPPlayerController* Controller;

	UPROPERTY()

	class AGameHUD* HUD;

	

	bool bThrowButtonPressed;

public:	

	void ThrowButtonPressed(bool bPressed);

};
