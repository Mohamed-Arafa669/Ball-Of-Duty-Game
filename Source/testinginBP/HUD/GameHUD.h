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

	void AddElimAnnouncement(FString Attacker, FString Victim);

protected:
	virtual void BeginPlay() override;


private:
	
	UPROPERTY()
		class APlayerController* OwningPlayer;

	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter);

	UPROPERTY(EditAnywhere, Category = "ElimAnnouncements")
	TSubclassOf<class UElimAnnouncements> ElimAnnouncementClass;

	UPROPERTY(EditAnywhere)
	float ElimAnnouncementTime = 3.f;
	UPROPERTY(EditAnywhere)
	float RespawnAnnouncementTime = 5.f;

	UFUNCTION()
	void ElimAnnouncementTimerFinished(UElimAnnouncements* MsgToRemove);

	UFUNCTION()
	void RespawnAnnouncementTimerFinished(UUI_RespawnWidget* MsgToRemove);


public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& package) { HUDPackage = package; }

};
