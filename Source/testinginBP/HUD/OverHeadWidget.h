// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverHeadWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API UOverHeadWidget : public UUserWidget
{
	GENERATED_BODY()

		UPROPERTY(meta = (BindWidget))
		class UTextBlock* displayRoleText;

		UPROPERTY(meta = (BindWidget))
		class UTextBlock* displayNameText;


	void SetDisplayText(FString roleTextToDisplay, FString nameTextToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* inPawn);

protected:
	virtual void OnLevelRemovedFromWorld(ULevel* inLevel, UWorld* inWorld) override;
};
