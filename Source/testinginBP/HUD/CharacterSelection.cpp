// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelection.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "testinginBP/PlayerController/CPPPlayerController.h"
#include "testinginBP/CharacterSelectInterface.h"
#include "Kismet/GameplayStatics.h"


void UCharacterSelection::NativeConstruct()
{
	Melody->SetVisibility(ESlateVisibility::Hidden);
	Crimson->SetVisibility(ESlateVisibility::Hidden);
	CrimsonBanner->SetVisibility(ESlateVisibility::Hidden);
	BTN_CrimsonMoreInfo->SetVisibility(ESlateVisibility::Hidden);
	MelodyBanner->SetVisibility(ESlateVisibility::Hidden);
	BTN_MelodyMoreInfo->SetVisibility(ESlateVisibility::Hidden);
	Tint->SetVisibility(ESlateVisibility::Hidden);
	MelodyMoreInfo->SetVisibility(ESlateVisibility::Hidden);
	CrimsonMoreInfo->SetVisibility(ESlateVisibility::Hidden);
	BTN_MelodyMoreInfo->SetVisibility(ESlateVisibility::Hidden);
	BTN_CrimsonMoreInfo->SetVisibility(ESlateVisibility::Hidden);
	BTN_MelodyExit->SetVisibility(ESlateVisibility::Hidden);
	BTN_CrimsonExit->SetVisibility(ESlateVisibility::Hidden);

	BTN_CageCharacter->SetIsEnabled(false);
	BTN_ShieldCharacter->SetIsEnabled(false);
	BTN_CloneCharacter->SetIsEnabled(false);
	BTN_TimeCharacter->SetIsEnabled(false);


}


bool UCharacterSelection::Initialize()
{
	bool Success = Super::Initialize();

	if (!Success) return false;

	if (!ensure(BTN_StealCharacter != nullptr)) return false;

	BTN_StealCharacter->OnClicked.AddDynamic(this, &UCharacterSelection::StealCharacterBTNPressed);

	if (!ensure(BTN_PowerCharacter != nullptr)) return false;

	BTN_PowerCharacter->OnClicked.AddDynamic(this, &UCharacterSelection::PowerCharacterBTNPressed);

	if (!ensure(BTN_ConfirmCharacterSelect != nullptr)) return false;

	BTN_ConfirmCharacterSelect->OnClicked.AddDynamic(this, &UCharacterSelection::ConfirmButtonSelected);

	if (!ensure(BTN_MelodyMoreInfo != nullptr)) return false;

	BTN_MelodyMoreInfo->OnClicked.AddDynamic(this, &UCharacterSelection::MelodyMoreInfoBTNPressed);

	if (!ensure(BTN_CrimsonMoreInfo != nullptr)) return false;

	BTN_CrimsonMoreInfo->OnClicked.AddDynamic(this, &UCharacterSelection::CrimsonMoreInfoBTNPressed);

	if (!ensure(BTN_MelodyExit != nullptr)) return false;

	BTN_MelodyExit->OnClicked.AddDynamic(this, &UCharacterSelection::MelodyInfoExitBTNPressed);

	if (!ensure(BTN_CrimsonExit != nullptr)) return false;

	BTN_CrimsonExit->OnClicked.AddDynamic(this, &UCharacterSelection::CrimsonInfoExitBTNPressed);

	return true;
}

void UCharacterSelection::StealCharacterBTNPressed()
{
	Melody->SetVisibility(ESlateVisibility::Visible);
	MelodyBanner->SetVisibility(ESlateVisibility::Visible);
	BTN_MelodyMoreInfo->SetVisibility(ESlateVisibility::Visible);

	Crimson->SetVisibility(ESlateVisibility::Hidden);
	CrimsonBanner->SetVisibility(ESlateVisibility::Hidden);
	BTN_CrimsonMoreInfo->SetVisibility(ESlateVisibility::Hidden);

	bMelodySelected = true;
	bCrimsonSelected = false;
}

void UCharacterSelection::PowerCharacterBTNPressed()
{
	Melody->SetVisibility(ESlateVisibility::Hidden);
	MelodyBanner->SetVisibility(ESlateVisibility::Hidden);
	BTN_MelodyMoreInfo->SetVisibility(ESlateVisibility::Hidden);


	Crimson->SetVisibility(ESlateVisibility::Visible);
	CrimsonBanner->SetVisibility(ESlateVisibility::Visible);
	BTN_CrimsonMoreInfo->SetVisibility(ESlateVisibility::Visible);

	bMelodySelected = false;
	bCrimsonSelected = true;
}


void UCharacterSelection::ConfirmButtonSelected()
{
	bConfirmed = true;

	
		 if (bConfirmed && bMelodySelected)
		 {
			 BTN_ConfirmCharacterSelect->SetIsEnabled(false);
			 BTN_PowerCharacter->SetIsEnabled(false);
			 BTN_StealCharacter->SetIsEnabled(false);
			 ButtonIndex = 1;
			 CallCharacterSelect();
		 }
		 else if (bConfirmed && bCrimsonSelected)
		 {
			 BTN_ConfirmCharacterSelect->SetIsEnabled(false);
			 BTN_PowerCharacter->SetIsEnabled(false);
			 BTN_StealCharacter->SetIsEnabled(false);
			 ButtonIndex = 2;
			 CallCharacterSelect();
		 }

	
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

void UCharacterSelection::MelodyMoreInfoBTNPressed()
{
	Tint->SetVisibility(ESlateVisibility::Visible);

	MelodyMoreInfo->SetVisibility(ESlateVisibility::Visible);
	CrimsonMoreInfo->SetVisibility(ESlateVisibility::Hidden);

	BTN_MelodyMoreInfo->SetVisibility(ESlateVisibility::Visible);
	BTN_CrimsonMoreInfo->SetVisibility(ESlateVisibility::Hidden);

	BTN_MelodyExit->SetVisibility(ESlateVisibility::Visible);
	BTN_CrimsonExit->SetVisibility(ESlateVisibility::Hidden);
}

void UCharacterSelection::CrimsonMoreInfoBTNPressed()
{
	Tint->SetVisibility(ESlateVisibility::Visible);

	MelodyMoreInfo->SetVisibility(ESlateVisibility::Hidden);
	CrimsonMoreInfo->SetVisibility(ESlateVisibility::Visible);

	BTN_MelodyMoreInfo->SetVisibility(ESlateVisibility::Hidden);
	BTN_CrimsonMoreInfo->SetVisibility(ESlateVisibility::Visible);

	BTN_MelodyExit->SetVisibility(ESlateVisibility::Hidden);
	BTN_CrimsonExit->SetVisibility(ESlateVisibility::Visible);
}

void UCharacterSelection::MelodyInfoExitBTNPressed()
{
	Tint->SetVisibility(ESlateVisibility::Hidden);
	MelodyMoreInfo->SetVisibility(ESlateVisibility::Hidden);
	CrimsonMoreInfo->SetVisibility(ESlateVisibility::Hidden);
	BTN_MelodyMoreInfo->SetVisibility(ESlateVisibility::Visible);
	BTN_CrimsonMoreInfo->SetVisibility(ESlateVisibility::Hidden);
	BTN_MelodyExit->SetVisibility(ESlateVisibility::Hidden);
	BTN_CrimsonExit->SetVisibility(ESlateVisibility::Hidden);
}

void UCharacterSelection::CrimsonInfoExitBTNPressed()
{
	Tint->SetVisibility(ESlateVisibility::Hidden);
	MelodyMoreInfo->SetVisibility(ESlateVisibility::Hidden);
	CrimsonMoreInfo->SetVisibility(ESlateVisibility::Hidden);
	BTN_MelodyMoreInfo->SetVisibility(ESlateVisibility::Hidden);
	BTN_CrimsonMoreInfo->SetVisibility(ESlateVisibility::Visible);
	BTN_MelodyExit->SetVisibility(ESlateVisibility::Hidden);
	BTN_CrimsonExit->SetVisibility(ESlateVisibility::Hidden);
}
