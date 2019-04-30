// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "GameFramework/GameStateBase.h"
#include "SoccerGameState.generated.h"

UENUM()
enum class ESoccerGameState : uint8
{
	GS_AwaitSpawnPlayers,
	GS_AwaitSpawnBall,
	
	GS_Playing,

	GS_AwaitReset,
};

UCLASS()
class SOCCERTYPE_API ASoccerGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 RedScore;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 BlueScore;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	ESoccerGameState GameState;
};
