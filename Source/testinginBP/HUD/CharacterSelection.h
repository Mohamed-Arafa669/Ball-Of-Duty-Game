// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelection.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API UCharacterSelection : public UUserWidget
{
	GENERATED_BODY()
protected:

	virtual bool Initialize();

	UFUNCTION()
		virtual void StealCharacterBTNPressed();

	UFUNCTION()
		virtual void PowerCharacterBTNPressed();

	UFUNCTION()
		virtual void MelodyMoreInfoBTNPressed();

	UFUNCTION()
		virtual void CrimsonMoreInfoBTNPressed();

	UFUNCTION()
		virtual void MelodyInfoExitBTNPressed();

	UFUNCTION()
		virtual void CrimsonInfoExitBTNPressed();

	UFUNCTION()
		virtual void ConfirmButtonSelected();

	virtual void NativeConstruct() override;

private:

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_StealCharacter;

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_PowerCharacter;

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_CageCharacter;

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_ShieldCharacter;

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_CloneCharacter;

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_TimeCharacter;

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_ConfirmCharacterSelect;

	UPROPERTY(meta = (BindWidget))
		class UImage* Melody;

	UPROPERTY(meta = (BindWidget))
		class UImage* Crimson;

	UPROPERTY(meta = (BindWidget))
		class UImage* MelodyBanner;

	UPROPERTY(meta = (BindWidget))
		class UImage* CrimsonBanner;

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_MelodyMoreInfo;

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_CrimsonMoreInfo;

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_MelodyExit;

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_CrimsonExit;

	UPROPERTY(meta = (BindWidget))
		class UImage* MelodyMoreInfo;

	UPROPERTY(meta = (BindWidget))
		class UImage* CrimsonMoreInfo;

	UPROPERTY(meta = (BindWidget))
		class UImage* Tint;

	UPROPERTY()
		uint8 ButtonIndex;


public:

	void Setup();
	void CloseWidget();
	void CallCharacterSelect();


	bool bConfirmed = false;
	bool bMelodySelected = false;
	bool bCrimsonSelected = false;


	
};
