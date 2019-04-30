// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "Blueprint/UserWidget.h"
#include "ScoreWidget.generated.h"

UCLASS()
class SOCCERTYPE_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetRedScore(int32 RedScore);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetBlueScore(int32 BlueScore);
};
