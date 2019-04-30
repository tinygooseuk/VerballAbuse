// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "SoccerGameViewportClient.h"

// Engine
#include "Engine/World.h"

// Game
#include "SoccerPlayerController.h"

bool USoccerGameViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	// We don't care about released events or repeating
	if (EventArgs.Event != EInputEvent::IE_Pressed)
	{
		return Super::InputKey(EventArgs);
	}

	// Get the key's name
	FString KeyName = EventArgs.Key.GetDisplayName().ToString();
	if (KeyName.Len() == 1 && FChar::IsAlnum(KeyName[0]))
	{
		UWorld* MyWorld = GetWorld();
		GOOSE_BAIL_RETURN(MyWorld, false);

		ASoccerPlayerController* PC = Cast<ASoccerPlayerController>(MyWorld->GetFirstPlayerController());
		GOOSE_BAIL_RETURN(PC, false);

		PC->OnReceiveTypedLetter(KeyName[0]);
	}
	
	// Not a sausage. Call to super.
	return Super::InputKey(EventArgs);
}