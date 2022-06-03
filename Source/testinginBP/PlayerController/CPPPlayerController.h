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
	void SetHUDHealth(float CurrentHealth, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);

	UPROPERTY()
	class AGameHUD* GameHUD;

protected:
	virtual void BeginPlay() override;
	//void SetHUDMatchCountdown(float CountdownTime);

};
