// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "WordWidget.h"

// Engine
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

// Game
#include "System/SoccerPlayerController.h"


UWordWidget::UWordWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{

}

void UWordWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterWithPlayerController();
}

void UWordWidget::InitWithWord(const FString& InWord, float InDifficulty)
{
	Word = InWord;
	Difficulty = InDifficulty;

	UHorizontalBox* Stack = GetStackWidget();
	GOOSE_BAIL(Stack);

	for (TCHAR Char : InWord.GetCharArray())
	{
		FText Text = FText::FromString(FString::Printf(TEXT("%c"), Char));

		UTextBlock* TextWidget = WidgetTree->ConstructWidget<UTextBlock>();
				
		FSlateFontInfo Font = TextWidget->Font;
		Font.Size = 60.0f;
		TextWidget->SetFont(Font);

		TextWidget->SetText(Text);

		UHorizontalBoxSlot* NewSlot = Stack->AddChildToHorizontalBox(TextWidget);
		NewSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		NewSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	}
}
bool UWordWidget::OnReceiveTypedLetter(TCHAR Letter)
{
	return false;
}
bool UWordWidget::IsCompleted() const
{
	return false;
}
float UWordWidget::GetProgress() const
{
	return 0.0f;
}
float UWordWidget::GetDifficulty() const
{
	return Difficulty;
}

void UWordWidget::RegisterWithPlayerController()
{
	ASoccerPlayerController* PC = GetFirstPlayerController();
	GOOSE_BAIL(PC);

	PC->RegisterNewWordWidget(this);
}

void UWordWidget::DeregisterWithPlayerController()
{
	ASoccerPlayerController* PC = GetFirstPlayerController();
	GOOSE_BAIL(PC);

	PC->DeregisterDeadWordWidget(this);
}

ASoccerPlayerController* UWordWidget::GetFirstPlayerController() const
{
	UWorld* World = GetWorld();
	GOOSE_BAIL_RETURN(World, nullptr);

	ASoccerPlayerController* PC = Cast<ASoccerPlayerController>(World->GetFirstPlayerController());
	GOOSE_BAIL_RETURN(PC, nullptr);

	return PC;
}