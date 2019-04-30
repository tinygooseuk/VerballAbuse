// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "GameFramework/Actor.h"
#include "SoccerPlayerStart.generated.h"

UCLASS()
class SOCCERTYPE_API ASoccerPlayerStart : public AActor
{
	GENERATED_BODY()

public:
	ASoccerPlayerStart();

private:
	UPROPERTY()
	class UCapsuleComponent* CapsuleComponent;
};
