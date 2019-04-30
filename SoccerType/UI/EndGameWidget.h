// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "Blueprint/UserWidget.h"
#include "EndGameWidget.generated.h"

UCLASS()
class SOCCERTYPE_API UEndGameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetText(const FString& InText);
	
};
