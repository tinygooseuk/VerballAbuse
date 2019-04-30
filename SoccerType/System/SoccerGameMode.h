// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "GameFramework/GameModeBase.h"
#include "Game/SoccerPlayer.h"
#include "SoccerGameMode.generated.h"

UCLASS()
class SOCCERTYPE_API ASoccerGameMode : public AGameModeBase
{
public:
	GENERATED_BODY()

	ASoccerGameMode();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void OnGoalScored(ETeam ScoringTeam);

private:
	///////////////////////// Resources /////////////////////////
	UPROPERTY()
	class UClass* UMG_ScoreWidget;

	UPROPERTY()
	class UClass* UMG_EndGameWidget;

	UPROPERTY()
	class USoundWave* SND_Goal;

	///////////////////////// Getters /////////////////////////
	class ASoccerGameState* GetSoccerGameState() const;

	///////////////////////// Functions /////////////////////////
	void RespawnPlayers();
	void SpawnBall();
	void AddEndGameUI(bool bWon);

	///////////////////////// State /////////////////////////
	float TransitionTimer;

	UPROPERTY()
	class UScoreWidget* ScoreWidget;

	UPROPERTY()
	class UEndGameWidget* EndGameWidget;
};
