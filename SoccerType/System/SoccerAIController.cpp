// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "SoccerAIController.h"

#include "Navigation/CrowdFollowingComponent.h"

ASoccerAIController::ASoccerAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

}
