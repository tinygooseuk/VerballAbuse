// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "GameFramework/PlayerController.h"
#include "SoccerPlayerController.generated.h"

USTRUCT()
struct FShotTargetInfo
{
	GENERATED_BODY()

	UPROPERTY()
	class ASoccerPlayer* Shooter;

	UPROPERTY()
	FVector Target;
};

USTRUCT()
struct FResolvedShotInfo
{
	GENERATED_BODY()

	void Reset()
	{
		Accuracy = 0.0f;
		TimeTaken = 0.0f;
		MistakesMade = 0;
		bSuccess = false;
	}

	UPROPERTY()
	float Accuracy;

	UPROPERTY()
	float TimeTaken;

	UPROPERTY()
	int32 MistakesMade;

	UPROPERTY()
	bool bSuccess;
};

DECLARE_DELEGATE_OneParam(FShotResultDelegate, FResolvedShotInfo/*, ResolvedShotInfo*/);

UCLASS()
class SOCCERTYPE_API ASoccerPlayerController : public APlayerController
{
public:
	GENERATED_BODY()

	ASoccerPlayerController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Called by GameportClient when receiving a keypress that looks like a letter, number, hyphen, etc.
	void OnReceiveTypedLetter(TCHAR Key);
	
	// Called by WordWidgets to register them
	void RegisterNewWordWidget(class UWordWidget* InWidget);
	// Called by WordWidgets to de-register them
	void DeregisterDeadWordWidget(class UWordWidget* InWidget);

	// Shake the screen with a given intensity and style
	void ShakeScreen(float Intensity, bool bIsError = false);

	// Pause/unpause for shot
	void PauseForInput(const FShotTargetInfo& ForShot, const FString& Prompt, FShotResultDelegate& Delegate);
	void UnPause(bool bShotSuccessful, float AccuracyModifier = 1.0f);

	UFUNCTION(BlueprintCallable)
	float CalculateAccuracy(float TimeTaken, int32 MistakesMade) const;

private:
	void BeginSlowmo(class ASoccerPlayer* FromPlayer);
	void EndSlowmo();

	void AddWordsToScreen(float TotalDifficulty);

	class ADecalActor* AddArrow(FVector From, FVector To, FLinearColor Colour) const;
		
	UFUNCTION()
	void RequestExit();

	///////////////////////// Resources /////////////////////////
	UPROPERTY()
	class UClass* TypingCameraShake_BP;
	UPROPERTY()
	class UClass* TypingErrorShake_BP;
	UPROPERTY()
	class UClass* UMG_FullWordWidget;
	UPROPERTY()
	class UClass* UMG_AccuracyOverlay;
	UPROPERTY()
	class UMaterialInterface* M_Arrow;
	UPROPERTY()
	class USoundWave* SND_Good;
	UPROPERTY()
	class USoundWave* SND_Terrible;
	UPROPERTY()
	class USoundWave* SND_Letter;
	UPROPERTY()
	class USoundWave* SND_BadLetter;

	///////////////////////// State /////////////////////////
	bool bSlowmoActive;
	float SlowmoTimerElapsed;
	int32 MistakesDuringSlowmo;

	FResolvedShotInfo ResolvedShotInfo;
	FShotResultDelegate ShotResultDelegate;

	///////////////////////// References /////////////////////////
	UPROPERTY()
	TWeakObjectPtr<class UAccuracyOverlay> AccuracyOverlay;

	UPROPERTY()
	TArray<TWeakObjectPtr<class UWordWidget>> WordWidgets;
};
