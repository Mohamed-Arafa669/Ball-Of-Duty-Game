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
		virtual void ConfirmButtonSelected();

private:

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_StealCharacter;

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_PowerCharacter;

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_ConfirmCharacterSelect;

	UPROPERTY()
		uint8 ButtonIndex;


public:

	void Setup();
	void CloseWidget();
	void CallCharacterSelect();

	bool bConfirmed = true;


	
};
