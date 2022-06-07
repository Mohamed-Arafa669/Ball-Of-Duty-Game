// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* crosshairCenter;
	UTexture2D* crosshairLeft;
	UTexture2D* crosshairRight; 
	UTexture2D* crosshairBottom;
	UTexture2D* crosshairTop; 
};
/**
 * 
 */
UCLASS()
class TESTINGINBP_API AGameHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		TSubclassOf<class UUserWidget> CharacterOverlayClass;


	void AddCharacterOverlay();

	UPROPERTY()
	class UCharacterOverlays* CharacterOverlay;

	UPROPERTY(EditAnywhere, Category = "Announcements")
		TSubclassOf<class UUserWidget> AnnouncementClass;

	UPROPERTY()
		class UAnnouncement* Announcement;

	void AddAnnouncement();

	UPROPERTY(EditAnywhere, Category = "Announcements")
		TSubclassOf<class UUserWidget> RespawnClass;

	UPROPERTY()
		class UUI_RespawnWidget* RespawnWidgets;

	void AddRespawnWidget();

protected:
	virtual void BeginPlay() override;


private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter);

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& package) { HUDPackage = package; }

};
