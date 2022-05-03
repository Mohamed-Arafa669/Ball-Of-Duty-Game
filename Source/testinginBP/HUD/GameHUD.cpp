// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"

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
