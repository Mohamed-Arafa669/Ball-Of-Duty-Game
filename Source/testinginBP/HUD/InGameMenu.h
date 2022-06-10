// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API UInGameMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void MenuSetup();
	void MenuTearDown();

protected:

	virtual bool Initialize() override;

	UFUNCTION()
		void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void OnPlayerLeftGame();

private:

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_ReturnToMenu;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_QuitGame;

	UFUNCTION()
	void ReturnButtonClicked();

	UFUNCTION()
	void QuitButtonClicked();

	UPROPERTY()
		class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY()
		class APlayerController* PlayerController;


};
