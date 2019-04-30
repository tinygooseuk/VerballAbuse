// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "Components/ActorComponent.h"
#include "SlowableDownableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOCCERTYPE_API USlowableDownableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USlowableDownableComponent();		

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;

	static void SetTimeDilation(float TimeDilation);

private:
	static TArray<USlowableDownableComponent*> AllSlowableDownables;
};
