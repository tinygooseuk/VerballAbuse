// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "SoccerPlayerStart.h"

// Engine
#include "Components/CapsuleComponent.h"

ASoccerPlayerStart::ASoccerPlayerStart()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComponent->InitCapsuleSize(50.0f, 160.0f);
	SetRootComponent(CapsuleComponent);
}