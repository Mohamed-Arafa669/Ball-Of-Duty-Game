// Fill out your copyright notice in the Description page of Project Settings.


#include "OverHeadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverHeadWidget::SetDisplayText(FString roleTextToDisplay, FString nameTextToDisplay)
{
	if (displayRoleText)
	{
		displayRoleText->SetText(FText::FromString(roleTextToDisplay));
	}
	if (displayNameText)
	{
		displayNameText->SetText(FText::FromString(nameTextToDisplay));
	}
}

void UOverHeadWidget::ShowPlayerNetRole(APawn* inPawn)
{
	ENetRole RemoteRole = inPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}
	const FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);

	const APlayerState* playerState = inPawn->GetPlayerState();
	FString playerName = "";

	if (playerState)
	{
		playerName = playerState->GetPlayerName();
	}

	SetDisplayText(RemoteRoleString, playerName);
}

void UOverHeadWidget::OnLevelRemovedFromWorld(ULevel* inLevel, UWorld* inWorld)
{
	RemoveFromParent();
	Super::OnLevelRemovedFromWorld(inLevel, inWorld);
}
