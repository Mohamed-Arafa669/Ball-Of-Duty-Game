// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API ACPPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void SetHUDScore(float Score);
protected:
	virtual void BeginPlay() override;
	//void SetHUDMatchCountdown(float CountdownTime);
private:
	class AGameHUD* GameHUD;
	UPROPERTY(VisibleAnywhere)
		class UCombatComponent* combat;

};
