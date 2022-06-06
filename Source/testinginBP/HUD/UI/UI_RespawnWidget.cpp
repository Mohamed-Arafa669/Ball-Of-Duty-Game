// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_RespawnWidget.h"
#include "testinginBP/Character/CPPTestCharacter.h"

void UUI_RespawnWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CreationTime = GetWorld()->GetTimeSeconds();
	CounterTextBlock->SetText(FText::FromString(FString::FromInt(CountdownTimeSeconds)));
}

void UUI_RespawnWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (GetWorld() && GetWorld()->TimeSince(CreationTime) > CountdownTimeSeconds) {
		if (ACPPTestCharacter* MyChar = Cast<ACPPTestCharacter>(GetOwningPlayerPawn()))
		{
			MyChar->RemoveWidget();

			/*FString lol = FString::Printf(TEXT("LOL"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, lol);*/
		}
	}

	int32 NewSeconds = CountdownTimeSeconds - GetWorld()->TimeSince(CreationTime);
	CounterTextBlock->SetText(FText::FromString(FString::FromInt(NewSeconds)));

}
