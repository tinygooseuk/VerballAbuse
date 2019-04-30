// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "Blueprint/UserWidget.h"
#include "WordWidget.generated.h"

UCLASS()
class SOCCERTYPE_API UWordWidget : public UUserWidget
{
public:
	GENERATED_BODY()

	UWordWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintCallable)
	virtual void InitWithWord(const FString& InWord, float InDifficulty); // Set idx = 0 also
	virtual bool OnReceiveTypedLetter(TCHAR Letter);

	UFUNCTION(BlueprintCallable)
	virtual bool IsCompleted() const;

	UFUNCTION(BlueprintCallable)
	virtual float GetProgress() const;

	float GetDifficulty() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	class UHorizontalBox* GetStackWidget() const;

protected:
	void RegisterWithPlayerController();
	void DeregisterWithPlayerController();

	FString Word;
	float Difficulty;

private:
	class ASoccerPlayerController* GetFirstPlayerController() const;
};
