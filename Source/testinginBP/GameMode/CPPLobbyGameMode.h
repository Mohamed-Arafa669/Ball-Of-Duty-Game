
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CPPLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGINBP_API ACPPLobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* newPlayer) override;

};