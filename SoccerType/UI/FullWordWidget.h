// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "WordWidget.h"
#include "FullWordWidget.generated.h"

UCLASS()
class SOCCERTYPE_API UFullWordWidget : public UWordWidget
{
public:
	GENERATED_BODY()

	UFullWordWidget(const FObjectInitializer& ObjectInitializer);

	virtual void InitWithWord(const FString& InWord, float InDifficulty) override; // Set idx = 0 also
	virtual bool OnReceiveTypedLetter(TCHAR Letter) override;
	virtual bool IsCompleted() const override;
	virtual float GetProgress() const override;

private:
	int32 CurrentIdx;
};
