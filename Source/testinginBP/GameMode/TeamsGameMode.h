// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameMode.h"
#include "TeamsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API ATeamsGameMode : public AMyGameMode
{
	GENERATED_BODY()

public:

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

protected:

	virtual void HandleMatchHasStarted() override;

};
