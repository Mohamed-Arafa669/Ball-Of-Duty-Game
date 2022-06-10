// Fill out your copyright notice in the Description page of Project Settings.


#include "ElimAnnouncements.h"
#include "Components/TextBlock.h"


void UElimAnnouncements::SetElimAnnouncemetText(FString AttackerName, FString VictimName)
{
	FString ElimAnnouncementText = FString::Printf(TEXT("%s Knocked %s"), *AttackerName, *VictimName);
	if (AnnouncementText)
	{
		AnnouncementText->SetText(FText::FromString(ElimAnnouncementText));
	}
}
