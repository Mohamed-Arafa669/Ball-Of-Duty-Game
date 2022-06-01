// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPPlayerController.h"
#include "testinginBP/HUD/GameHUD.h"
#include "testinginBP/HUD/CharacterOverlays.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"



//void ACPPPlayerController::SetHUDMatchCountdown(float CountdownTime)
//{
//	GameHUD = GameHUD
//}

void ACPPPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	GameHUD = Cast<AGameHUD>(GetHUD());
}

void ACPPPlayerController::SetHUDScore(float Score)
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetHUD()) : GameHUD;
	bool bHUDValidations = GameHUD &&
		GameHUD->CharacterOverlay &&
		GameHUD->CharacterOverlay->ScoreAmount;
	
	if (bHUDValidations)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		GameHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}

}