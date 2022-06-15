// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelection.h"
#include "Components/Button.h"
#include "testinginBP/PlayerController/CPPPlayerController.h"
#include "testinginBP/CharacterSelectInterface.h"
#include "Kismet/GameplayStatics.h"

bool UCharacterSelection::Initialize()
{
	bool Success = Super::Initialize();

	if (!Success) return false;

	if (!ensure(BTN_StealCharacter != nullptr)) return false;

	BTN_StealCharacter->OnClicked.AddDynamic(this, &UCharacterSelection::StealCharacterBTNPressed);

	if (!ensure(BTN_PowerCharacter != nullptr)) return false;

	BTN_PowerCharacter->OnClicked.AddDynamic(this, &UCharacterSelection::PowerCharacterBTNPressed);


	return true;
}

void UCharacterSelection::StealCharacterBTNPressed()
{
	ButtonIndex = 1;
	CallCharacterSelect();
	CloseWidget();
}

void UCharacterSelection::PowerCharacterBTNPressed()
{
	ButtonIndex = 2;
	CallCharacterSelect();
	CloseWidget();

}



void UCharacterSelection::Setup()
{
	this->AddToViewport();

	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();


		if (PlayerController != nullptr)
		{
			FInputModeUIOnly InputModeData;

			InputModeData.SetWidgetToFocus(this->TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PlayerController->SetInputMode(InputModeData);

			PlayerController->bShowMouseCursor = true;

		}
	}
}

void UCharacterSelection::CloseWidget()
{
	this->RemoveFromViewport();

	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();


		if (PlayerController != nullptr)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);

			PlayerController->bShowMouseCursor = false;

		}
	}
}

void UCharacterSelection::CallCharacterSelect()
{
	ACPPPlayerController* PlayerController = Cast<ACPPPlayerController>(GetWorld()->GetFirstPlayerController());

	PlayerController->ServerSelectedCharacter(ButtonIndex);
}
