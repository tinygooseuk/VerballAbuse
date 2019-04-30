// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "SoccerGameMode.h"

// Engine
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"
#include "UserWidget.h"

// Game
#include "FX/ExplosionEmitter.h"
#include "Game/SoccerBall.h"
#include "Game/SoccerPlayerStart.h"
#include "System/SoccerGameState.h"
#include "System/SoccerPlayerController.h"
#include "UI/EndGameWidget.h"
#include "UI/ScoreWidget.h"

constexpr int32 MAX_SCORE = 10; //TODO: make 10

ASoccerGameMode::ASoccerGameMode()
	: TransitionTimer(0.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerControllerClass = ASoccerPlayerController::StaticClass();
	GameStateClass = ASoccerGameState::StaticClass();

	// Resources
	UMG_ScoreWidget = FIND_CLASS(UMG_ScoreWidget, TEXT("UMG"));
	UMG_EndGameWidget = FIND_CLASS(UMG_EndGameWidget, TEXT("UMG"));
	SND_Goal = FIND_RESOURCE(SoundWave, Goal, TEXT("Sounds"));
}

void ASoccerGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Create score UI
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	APlayerController* PC = World->GetFirstPlayerController();
	GOOSE_BAIL(PC);

	ScoreWidget = CreateWidget<UScoreWidget>(PC, UMG_ScoreWidget);
	ScoreWidget->AddToPlayerScreen();
}

void ASoccerGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TransitionTimer += DeltaSeconds;

	ESoccerGameState& State = GetSoccerGameState()->GameState;
	switch (State)
	{
		case ESoccerGameState::GS_AwaitSpawnPlayers:
		{
			RespawnPlayers();

			State = ESoccerGameState::GS_AwaitSpawnBall;
		}

		case ESoccerGameState::GS_AwaitSpawnBall:
		{
			if (TransitionTimer > 1.0f)
			{
				TransitionTimer = 0.0f;

				SpawnBall();
				State = ESoccerGameState::GS_Playing;
			}
		}

		case ESoccerGameState::GS_AwaitReset:
		{
			if (TransitionTimer > 4.0f)
			{				
				TransitionTimer = 0.0f;

				State = ESoccerGameState::GS_AwaitSpawnPlayers;
			}
		}
		break;
	}
}

void ASoccerGameMode::OnGoalScored(ETeam ScoringTeam)
{
	ASoccerGameState* SoccerGameState = GetSoccerGameState();
	GOOSE_BAIL(SoccerGameState);

	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	// Update score UI
	if (ScoringTeam == ETeam::TEAM_Red)
	{
		SoccerGameState->RedScore++;
		ScoreWidget->SetRedScore(SoccerGameState->RedScore);
	}
	else
	{
		SoccerGameState->BlueScore++;
		ScoreWidget->SetBlueScore(SoccerGameState->BlueScore);
	}

	// Do a sound
	UGameplayStatics::PlaySound2D(this, SND_Goal, 1.0f, FMath::RandRange(0.8f, 1.2f));

	// Kill all players & balls
	for (TActorIterator<ASoccerPlayer> It(World); It; ++It)
	{
		// Make a little explosion too
		AExplosionEmitter::SpawnExplosion(this, ASoccerPlayer::GetColourForTeam(It->GetTeam()), It->GetActorLocation(), FRotator::ZeroRotator, 0.25f);
		It->Destroy();
	}

	for (TActorIterator<ASoccerBall> It(World); It; ++It)
	{
		AExplosionEmitter::SpawnExplosion(this, FLinearColor::White, It->GetActorLocation(), FRotator::ZeroRotator, 0.25f);

		FLinearColor ScoringColour = ASoccerPlayer::GetColourForTeam(ScoringTeam);
		FRotator Orientation = FRotator(40.0f * (ScoringTeam == ETeam::TEAM_Red ? +1.0f : -1.0f), 0.0f, 0.0f);

		AExplosionEmitter::SpawnExplosion(this, ScoringColour, It->GetActorLocation(), Orientation, 1.5f);

		It->Destroy();
	}

	if (SoccerGameState->RedScore >= MAX_SCORE)
	{
		// You win!
		AddEndGameUI(true);
	}
	else if (SoccerGameState->BlueScore >= MAX_SCORE)
	{
		// You lose!
		AddEndGameUI(false);
	}
	else
	{
		// Get ready to restart
		TransitionTimer = 0.0f;
		SoccerGameState->GameState = ESoccerGameState::GS_AwaitReset;
	}
}

ASoccerGameState* ASoccerGameMode::GetSoccerGameState() const
{
	return GetGameState<ASoccerGameState>();
}

void ASoccerGameMode::RespawnPlayers()
{
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	for (TActorIterator<ASoccerPlayerStart> It(World); It; ++It)
	{
		FVector Location = It->GetActorLocation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ASoccerPlayer* NewPlayer = World->SpawnActor<ASoccerPlayer>(ASoccerPlayer::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);

		// Make a little explosion too
		AExplosionEmitter::SpawnExplosion(this, ASoccerPlayer::GetColourForTeam(NewPlayer->GetTeam()), Location, FRotator::ZeroRotator, 0.5f);
	}
}
void ASoccerGameMode::SpawnBall()
{
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	FVector Location = FVector(0.0f, 0.0f, 10'00.0f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	World->SpawnActor<ASoccerBall>(ASoccerBall::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);
}
void ASoccerGameMode::AddEndGameUI(bool bWon)
{
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	APlayerController* PC = World->GetFirstPlayerController();
	GOOSE_BAIL(PC);

	// Add title widget
	EndGameWidget = CreateWidget<UEndGameWidget>(PC, UMG_EndGameWidget);
	EndGameWidget->SetText(bWon ? TEXT("You Win!") : TEXT("You Lost!"));
	EndGameWidget->AddToPlayerScreen();
}