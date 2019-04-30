// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "GameFramework/Character.h"
#include "SoccerPlayer.generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	TEAM_Red = 0,
	TEAM_Blue
};

UCLASS()
class SOCCERTYPE_API ASoccerPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	ASoccerPlayer();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	///////////////////////// Team funcs /////////////////////////
	void SetTeam(ETeam Team);
	UFUNCTION(BlueprintCallable)
	ETeam GetTeam() const;
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsRedTeam() const { return GetTeam() == ETeam::TEAM_Red; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsBlueTeam() const { return GetTeam() == ETeam::TEAM_Blue; }

	///////////////////////// Dribble funcs //////////////////////////////
	void NotifyDribbling(); 
	void NotifyNoLongerDribbling();
	bool IsDribbling() const;
	bool IsEligibleForDribble() const;
	
	///////////////////////// Shots /////////////////////////
	UFUNCTION(BlueprintCallable)
	void Pass();
	UFUNCTION(BlueprintCallable)
	void Shoot();

	static FLinearColor GetColourForTeam(ETeam Team);

private:
	///////////////////////// Private funcs /////////////////////////
	int32 GetNumberOfPasses() const;
	void SetNumberOfPasses(int32 PassesBeforeShooting);

	bool KickBall(FVector KickImpulse, float Accuracy = 1.0f);

	class UBlackboardComponent* GetBlackboard() const;

	///////////////////////// State //////////////////////////////
	ETeam Team;
	float DribbleTimer;
	bool bIsDribbling;

	///////////////////////// Resources /////////////////////////
	UPROPERTY()
	class UBehaviorTree* BT_SoccerPlayer;

	UPROPERTY()
	class UPaperFlipbook* T_Player_Flipbook;

	UPROPERTY()
	class USoundWave* SND_Kick;

	///////////////////////// Components /////////////////////////
	UPROPERTY()
	class UPaperFlipbookComponent* FlipbookComponent;

	UPROPERTY()
	class USlowableDownableComponent* SlowableComponent;
};
