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
	
	//////////////////////////////////////////////////////////////////////////
	///	
	/// Character Selection	
	/// //////////////////////////////////////////////////////////////////////////
	/// 
	/*UPROPERTY(EditAnywhere, Category = "CharacterSelection")
		TSubclassOf<class UUserWidget> CharacterSelectionClass;

	UPROPERTY()
		class UCharacterSelection* CharacterSelection;

	void AddCharacterSelect();*/

/// <summary>
/// HUD ?
/// </summary>
/// 

	UPROPERTY()
		class UCrimsonProfiler* CrimsonProfiler;

	UPROPERTY(EditAnywhere, Category = "Announcements")
		TSubclassOf<class UUserWidget> CrimsonProfilerClass;

	void AddCrimsonProfiler();


	UPROPERTY()
		class UMelodyProfiler* MelodyOverlay;

	UPROPERTY(EditAnywhere, Category = "Announcements")
		TSubclassOf<class UUserWidget> MelodyProfilerClass;

	void AddMelodyProfiler();

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
	

	UFUNCTION()
	void ElimAnnouncementTimerFinished(UElimAnnouncements* MsgToRemove);

	UPROPERTY()
	TArray<UElimAnnouncements*> ElimMessages;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& package) { HUDPackage = package; }

};
