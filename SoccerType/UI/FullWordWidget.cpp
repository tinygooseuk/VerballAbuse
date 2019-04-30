// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "FullWordWidget.h"

// Engine
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"

// Game
#include "System/SoccerPlayerController.h"

UFullWordWidget::UFullWordWidget(const FObjectInitializer& ObjectInitializer)
	: UWordWidget(ObjectInitializer)
	, CurrentIdx(0)	
{

}

void UFullWordWidget::InitWithWord(const FString& InWord, float InDifficulty)
{
	Super::InitWithWord(InWord, InDifficulty);

	// Also reset index
	CurrentIdx = 0;
}

bool UFullWordWidget::OnReceiveTypedLetter(TCHAR Letter)
{
	Super::OnReceiveTypedLetter(Letter);

	if (IsCompleted())
	{
		return false;
	}

	if (Letter == FChar::ToLower(Word[CurrentIdx]))
	{
		UHorizontalBox* StackWidget = GetStackWidget();
		GOOSE_BAIL_RETURN(StackWidget, false);

		do
		{
			UHorizontalBoxSlot* StackSlot = static_cast<UHorizontalBoxSlot*>(StackWidget->GetSlots()[CurrentIdx]);
		
			UTextBlock* TextBlock = Cast<UTextBlock>(StackSlot->Content);
			GOOSE_BAIL_RETURN(TextBlock, false);

			FSlateFontInfo Font = TextBlock->Font;
			Font.Size = 80.0f;
			TextBlock->SetFont(Font);
			TextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Green));

			CurrentIdx++;
		} while (CurrentIdx < Word.Len() && !FChar::IsAlpha(Word[CurrentIdx]));

		UWorld* World = GetWorld();
		GOOSE_BAIL_RETURN(World, false);

		ASoccerPlayerController* PC = Cast<ASoccerPlayerController>(World->GetFirstPlayerController());
		GOOSE_BAIL_RETURN(PC, false);

		if (CurrentIdx >= Word.Len())
		{
			PC->ShakeScreen(1.0f);
		}
		else
		{
			PC->ShakeScreen(0.2f);
		}

		return true;
	}

	return false;
}

bool UFullWordWidget::IsCompleted() const 
{
	return CurrentIdx >= Word.Len();
}

float UFullWordWidget::GetProgress() const
{
	return float(CurrentIdx) / float(Word.Len());
}