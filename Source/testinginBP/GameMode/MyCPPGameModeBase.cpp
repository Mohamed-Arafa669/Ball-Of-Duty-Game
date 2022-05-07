// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCPPGameModeBase.h"
#include "Net/UnrealNetwork.h"

AMyCPPGameModeBase::AMyCPPGameModeBase() 
{

}

void AMyCPPGameModeBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyCPPGameModeBase, CurrentPawnToAssign);
}

UClass* AMyCPPGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (CurrentPawnToAssign)
	{
		if (FirstPawn != nullptr && SecondPawn != nullptr)
		{
			if (CurrentPawnToAssign == FirstPawn)
			{
				CurrentPawnToAssign = SecondPawn;
			}
			else
			{
				CurrentPawnToAssign = FirstPawn;
			}
		}
	}
	else
	{
		if (FirstPawn != nullptr && SecondPawn != nullptr)
		{
			CurrentPawnToAssign = (true) ? SecondPawn : FirstPawn;
		}
	}
	return CurrentPawnToAssign;
}
