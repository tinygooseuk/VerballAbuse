// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "AIController.h"
#include "SoccerAIController.generated.h"

/**
 * 
 */
UCLASS()
class SOCCERTYPE_API ASoccerAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASoccerAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
