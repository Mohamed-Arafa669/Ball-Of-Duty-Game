// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"
#include "GameFramework/PlayerController.h"
#include "Announcement.h"
#include "testinginBP/HUD/UI/UI_RespawnWidget.h"
#include "CharacterOverlays.h"

void AGameHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D viewportCenter(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);

		if (HUDPackage.crosshairCenter)
		{
			DrawCrosshair(HUDPackage.crosshairCenter, viewportCenter);
		}
		if (HUDPackage.crosshairLeft)
		{
			DrawCrosshair(HUDPackage.crosshairLeft, viewportCenter);
		}
		if (HUDPackage.crosshairRight)
		{
			DrawCrosshair(HUDPackage.crosshairRight, viewportCenter);
		}
		if (HUDPackage.crosshairBottom)
		{
			DrawCrosshair(HUDPackage.crosshairBottom, viewportCenter);
		}
		if (HUDPackage.crosshairTop)
		{
			DrawCrosshair(HUDPackage.crosshairTop, viewportCenter);
		}
		
		
		
	}
}

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();
	//AddCharacterOverlay();
}

void AGameHUD::AddCharacterOverlay()
{
	APlayerController* playerController = GetOwningPlayerController();
	if (playerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlays>(playerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void AGameHUD::AddAnnouncement()
{
	APlayerController* playerController = GetOwningPlayerController();
	if (playerController && AnnouncementClass)
	{
		Announcement = CreateWidget<UAnnouncement>(playerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

void AGameHUD::AddRespawnWidget()
{
	APlayerController* playerController = GetOwningPlayerController();
	if (playerController && RespawnClass)
	{
		/*RespawnWidgets = CreateWidget<UUI_RespawnWidget>(playerController, RespawnClass);
		if (playerController && RespawnWidgetClass)
		{
			RespawnWidgets = CreateWidget<UUI_RespawnWidget>(playerController, RespawnWidgetClass);
			RespawnWidgets->AddToViewport();
		}*/
	}
}

void AGameHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter)
{
	const float	 textureWidth = Texture->GetSizeX();
	const float	 textureHeight = Texture->GetSizeY();
	const FVector2D textureDrawPoint(
		ViewportCenter.X - (textureWidth/2.0f),
		ViewportCenter.Y - (textureHeight/2.0f)
	);

	DrawTexture(
		Texture,
		textureDrawPoint.X,
		textureDrawPoint.Y,
		textureWidth,
		textureHeight,
		0.0f,0.0f,1.0f,1.0f,
		FLinearColor::Red
	);
}
