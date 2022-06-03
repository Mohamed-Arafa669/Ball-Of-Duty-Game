// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPPlayerController.h"
#include "testinginBP/HUD/GameHUD.h"
#include "testinginBP/HUD/CharacterOverlays.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"



//void ACPPPlayerController::SetHUDMatchCountdown(float CountdownTime)
//{
//	GameHUD = GameHUD
//}

void ACPPPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	GameHUD = Cast<AGameHUD>(GetHUD());
	
}

void ACPPPlayerController::SetHUDHealth(float CurrentHealth, float MaxHealth)
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetHUD()) : GameHUD;
	if (GameHUD && GameHUD->CharacterOverlay && GameHUD->CharacterOverlay->HealthBar)
	{
		const float HealthPercent = CurrentHealth / MaxHealth;
		GameHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
	}
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

void ACPPPlayerController::SetHUDDefeats(int32 Defeats)
{
	GameHUD = GameHUD == nullptr ? Cast<AGameHUD>(GetHUD()) : GameHUD;
	bool bHUDValidations = GameHUD &&
		GameHUD->CharacterOverlay &&
		GameHUD->CharacterOverlay->DefeatsAmount;

	if (bHUDValidations)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"),Defeats);
		GameHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}

}
