// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMG.h"
#include "UI_RespawnWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API UUI_RespawnWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
		UTextBlock* CounterTextBlock;

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	float CreationTime;
	float CountdownTimeSeconds;
	
};
