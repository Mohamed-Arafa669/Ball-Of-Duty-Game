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

protected:
	virtual void BeginPlay() override;
	void ThrowButtonPressed(bool bPressed);

	

	void SetHUDCrosshairs(float DeltaTime);

private:
	class ACPPTestCharacter* character;
	class ACPPPlayerController* Controller;
	class AGameHUD* HUD;

	UPROPERTY(Replicated)
	ACPPBall* eqippedBall;

	bool bThrowButtonPressed;

public:	

		
};
