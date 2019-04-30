// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "Engine/GameViewportClient.h"
#include "SoccerGameViewportClient.generated.h"

UCLASS()
class SOCCERTYPE_API USoccerGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
	
public:
	virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;
};
