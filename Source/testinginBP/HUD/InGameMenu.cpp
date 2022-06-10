// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/Button.h"
#include "MultiplayerSessions/Public/MultiplayerSessionsSubsystem.h"
#include "testinginBP/Character/CPPTestCharacter.h"

void UInGameMenu::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameAndUI InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);

		}
	}
	if (BTN_ReturnToMenu && !BTN_ReturnToMenu->OnClicked.IsBound())
	{
		BTN_ReturnToMenu->OnClicked.AddDynamic(this, &UInGameMenu::ReturnButtonClicked);
	}

	if (BTN_QuitGame && !BTN_QuitGame->OnClicked.IsBound())
	{
		BTN_QuitGame->OnClicked.AddDynamic(this, &UInGameMenu::QuitButtonClicked);
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>();
		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UInGameMenu::OnDestroySession);
		}
	}
}

bool UInGameMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	return true;
}


void UInGameMenu::OnDestroySession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		BTN_ReturnToMenu->SetIsEnabled(true);
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		AGameModeBase* GameModeBase = World->GetAuthGameMode<AGameModeBase>();
		if (GameModeBase)
		{
			GameModeBase->ReturnToMainMenuHost();
		}
		else
		{
			PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
			if (PlayerController)
			{
				PlayerController->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	}
}



void UInGameMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);

		}
	}
	if (BTN_ReturnToMenu && BTN_ReturnToMenu->OnClicked.IsBound())
	{
		BTN_ReturnToMenu->OnClicked.RemoveDynamic(this, &UInGameMenu::ReturnButtonClicked);
	}

	if (BTN_QuitGame && BTN_QuitGame->OnClicked.IsBound())
	{
		BTN_QuitGame->OnClicked.RemoveDynamic(this, &UInGameMenu::QuitButtonClicked);
	}
	if (MultiplayerSessionsSubsystem && MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsBound())
	{
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UInGameMenu::OnDestroySession);

	}
}

void UInGameMenu::ReturnButtonClicked()
{
	BTN_ReturnToMenu->SetIsEnabled(false);

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* FirstPlayerController = World->GetFirstPlayerController();
		if (FirstPlayerController)
		{
			ACPPTestCharacter* Character = Cast<ACPPTestCharacter>(FirstPlayerController->GetPawn());

			if (Character)
			{
				Character->ServerLeaveGame();
				Character->OnLeftGame.AddDynamic(this, &UInGameMenu::OnPlayerLeftGame);
				if (MultiplayerSessionsSubsystem)
				{
					MultiplayerSessionsSubsystem->DestroySession();
					//UGameplayStatics::OpenLevel(GetWorld(), FName("GameStartupMap1"));

				}
			}
			else
			{
				BTN_ReturnToMenu->SetIsEnabled(true);
			}
		}
	}
	
}

void UInGameMenu::QuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(),GetWorld()->GetFirstPlayerController(),EQuitPreference::Quit,false);
}

void UInGameMenu::OnPlayerLeftGame()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->DestroySession();
		//UGameplayStatics::OpenLevel(GetWorld(), FName("GameStartupMap1"));

	}
}