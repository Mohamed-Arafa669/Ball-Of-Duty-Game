// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ElimAnnouncements.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API UElimAnnouncements : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetElimAnnouncemetText(FString AttackerName, FString VictimName);

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* AnnouncementBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AnnouncementText;
	
};
