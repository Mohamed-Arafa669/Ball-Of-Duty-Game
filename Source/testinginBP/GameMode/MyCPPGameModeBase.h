// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "MyCPPGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API AMyCPPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMyCPPGameModeBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyGame")
		TSubclassOf<class ACharacter> FirstPawn; //lw mnf3sh ACharacter 5leha ACPPTestCharacter

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyGame")
		TSubclassOf<class ACharacter> SecondPawn; //lw mnf3sh ACharacter 5leha ACPPTestCharacter

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //Replication

protected:

	UPROPERTY (Replicated)
		TSubclassOf<class ACharacter> CurrentPawnToAssign;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
};
