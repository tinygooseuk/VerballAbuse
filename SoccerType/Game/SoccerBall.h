// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "GameFramework/Actor.h"
#include "SoccerBall.generated.h"

UCLASS()
class SOCCERTYPE_API ASoccerBall : public AActor
{
	GENERATED_BODY()
	
public:	
	ASoccerBall();

	// AActor
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	///////////////////////// Events /////////////////////////
	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	///////////////////////// Public API /////////////////////////
	UFUNCTION(BlueprintCallable)
	void AttachDribbler(class ASoccerPlayer* Player);
	UFUNCTION(BlueprintCallable)
	void DetachDribbler();

	UFUNCTION(BlueprintCallable)
	class ASoccerPlayer* GetDribbler() const;

private:
	UPROPERTY()
	TWeakObjectPtr<class ASoccerPlayer> Dribbler;

	///////////////////////// State /////////////////////////
	float DribbleDebounceTimer;

	///////////////////////// Components /////////////////////////
	UPROPERTY()
	class UStaticMeshComponent* BallMeshComponent;

	UPROPERTY()
	class USphereComponent* BallDribbleDetectorComponent;

	UPROPERTY()
	class USlowableDownableComponent* SlowableComponent;
};
