// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "Blueprint/UserWidget.h"
#include "AccuracyOverlay.generated.h"

UCLASS()
class SOCCERTYPE_API UAccuracyOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetCurrentAccuracy(float Accuracy);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetFinalAccuracy(float Accuracy);
};
