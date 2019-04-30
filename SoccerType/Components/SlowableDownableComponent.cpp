// (c) 2019 TinyGoose Ltd., All Rights Reserved.
#include "SlowableDownableComponent.h"

#include "GameFramework/Actor.h"

TArray<USlowableDownableComponent*> USlowableDownableComponent::AllSlowableDownables;

USlowableDownableComponent::USlowableDownableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void USlowableDownableComponent::BeginPlay()
{
	Super::BeginPlay();

	AllSlowableDownables.Add(this);
}
void USlowableDownableComponent::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

	AllSlowableDownables.Remove(this);
}

void USlowableDownableComponent::SetTimeDilation(float TimeDilation)
{
	for (USlowableDownableComponent* Slowable : AllSlowableDownables)
	{
		AActor* Owner = Slowable->GetOwner();
		GOOSE_BAIL_CONTINUE(Owner);

		Owner->CustomTimeDilation = TimeDilation;
	}
}