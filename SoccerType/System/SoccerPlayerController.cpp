// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "SoccerPlayerController.h"

// Engine
#include "Components/DecalComponent.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Engine/DecalActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Sound/SoundWave.h"

// Game
#include "Components/SlowableDownableComponent.h"
#include "Data/WordsDatabase.h"
#include "Game/SoccerPlayer.h"
#include "UI/AccuracyOverlay.h" 
#include "UI/FullWordWidget.h" 
#include "UI/WordWidget.h"

#include "Shared/Tween/GooseTweenComponent.h"

constexpr float MAX_SLOWMO_TIME = 5.0f;

ASoccerPlayerController::ASoccerPlayerController()
	: bSlowmoActive(false)
	, SlowmoTimerElapsed(0.0f)
	, MistakesDuringSlowmo(0)
	, ShotResultDelegate(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	TypingCameraShake_BP = FIND_CLASS(TypingCameraShake_BP, TEXT("FX"));
	TypingErrorShake_BP = FIND_CLASS(TypingErrorShake_BP, TEXT("FX"));

	M_Arrow = FIND_RESOURCE(MaterialInterface, M_Arrow, TEXT("Materials"));

	UMG_FullWordWidget = FIND_CLASS(UMG_FullWordWidget, TEXT("UMG"));
	UMG_AccuracyOverlay = FIND_CLASS(UMG_AccuracyOverlay, TEXT("UMG"));

	SND_Good = FIND_RESOURCE(SoundWave, Good, TEXT("Sounds"));
	SND_Terrible = FIND_RESOURCE(SoundWave, Terrible, TEXT("Sounds"));
	SND_Letter = FIND_RESOURCE(SoundWave, Letter, TEXT("Sounds"));
	SND_BadLetter = FIND_RESOURCE(SoundWave, BadLetter, TEXT("Sounds"));
}

void ASoccerPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GOOSE_BAIL(InputComponent);

	static const FName NAME_Quit = TEXT("Quit");
	InputComponent->BindAction(NAME_Quit, EInputEvent::IE_Pressed, this, &ASoccerPlayerController::RequestExit);
}

void ASoccerPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bSlowmoActive)
	{
		SlowmoTimerElapsed += DeltaSeconds;

		float CurrentAccuracy = CalculateAccuracy(SlowmoTimerElapsed, MistakesDuringSlowmo);

		// Update bar overlay
		if (AccuracyOverlay.IsValid())
		{
			AccuracyOverlay->SetCurrentAccuracy(CurrentAccuracy);
		}

		// Too long? Cancel!
		if (CurrentAccuracy == 0.0f || SlowmoTimerElapsed > MAX_SLOWMO_TIME)
		{
			// Play bad sound
			UGameplayStatics::PlaySound2D(this, SND_Terrible, 1.0f, FMath::RandRange(0.8f, 1.2f));

			// Unpause
			UnPause(false);
		}
	}
}


void ASoccerPlayerController::OnReceiveTypedLetter(TCHAR Key) 
{
	if (WordWidgets.Num() == 0)
	{
		return;
	}

	bool bHandled = false;
	   
	for (int32 I = 0; I < WordWidgets.Num(); I++)
	{
		if (!WordWidgets[I].IsValid())
		{
			WordWidgets.RemoveAt(I--);
			continue;
		}

		bool bThisHandled = WordWidgets[I]->OnReceiveTypedLetter(FChar::ToLower(Key));
		bHandled |= bThisHandled;

		if (bThisHandled)
		{
			// Play key sound
			float Progress = WordWidgets[I]->GetProgress();
			UGameplayStatics::PlaySound2D(this, SND_Letter, 1.0f, 1.0f + (Progress * 0.4f));
		}

		// Check iff done
		if (WordWidgets[I]->IsCompleted())
		{
			// Get word entry from widget
			float Difficulty = WordWidgets[I]->GetDifficulty();

			// Remove word
			WordWidgets[I]->RemoveFromParent();
			WordWidgets.RemoveAt(I--);
			
			// Play good sound
			float Accuracy = CalculateAccuracy(SlowmoTimerElapsed, MistakesDuringSlowmo);
			UGameplayStatics::PlaySound2D(this, SND_Good, FMath::Min(1.0f, Accuracy * 2.0f), FMath::RandRange(0.8f, 1.2f));

			// End slowmo
			UnPause(true, Difficulty);
		}
	}

	// Wrong key!
	if (!bHandled)
	{
		// Shake a "no" gesture
		ShakeScreen(0.5f, true);

		// Play bad sound
		UGameplayStatics::PlaySound2D(this, SND_BadLetter, 1.0f, FMath::RandRange(0.8f, 1.2f));

		// Count mistake
		MistakesDuringSlowmo++;
	}
}

void ASoccerPlayerController::RegisterNewWordWidget(class UWordWidget* InWidget)
{
	WordWidgets.Add(InWidget);
}
void ASoccerPlayerController::DeregisterDeadWordWidget(class UWordWidget* InWidget)
{
	WordWidgets.RemoveSingle(InWidget);
}

void ASoccerPlayerController::ShakeScreen(float Intensity, bool bIsError)
{
	APlayerCameraManager* CamManager = PlayerCameraManager;
	GOOSE_BAIL(CamManager);

	CamManager->PlayCameraShake(bIsError ? TypingErrorShake_BP : TypingCameraShake_BP, Intensity);
}

void ASoccerPlayerController::PauseForInput(const FShotTargetInfo& ForShot, const FString& Prompt, FShotResultDelegate& Delegate)
{
	// Double negative so we get a trace when this is double called!
	if (!GOOSE_VERIFY(!bSlowmoActive))
	{
		return;
	}

	// Add arrow on pitch
	FLinearColor TeamColour = ASoccerPlayer::GetColourForTeam(ForShot.Shooter->GetTeam());
	AddArrow(ForShot.Shooter->GetActorLocation(), ForShot.Target, TeamColour);

	if (ForShot.Shooter->GetTeam() == ETeam::TEAM_Blue)
	{
		// AI - kick immediately
		FResolvedShotInfo ImmediateShot;
		ImmediateShot.Accuracy = FMath::FRandRange(0.7f, 1.0f);
		ImmediateShot.bSuccess = true;
		ImmediateShot.MistakesMade = 0;
		ImmediateShot.TimeTaken = 0.5f;

		Delegate.Execute(ImmediateShot);
		return;
	}

	ShotResultDelegate = Delegate;
	ResolvedShotInfo.Reset();
	
	// Start being slowmo
	BeginSlowmo(ForShot.Shooter);

	// Add scoring widgets
	AddWordsToScreen(5.0f);

	// Add accuracy overlay
	AccuracyOverlay = CreateWidget<UAccuracyOverlay>(this, UMG_AccuracyOverlay);
	AccuracyOverlay->AddToPlayerScreen();
}

void ASoccerPlayerController::UnPause(bool bShotSuccessful, float AccuracyModifier)
{
	float Accuracy = FMath::Min(1.0f, CalculateAccuracy(SlowmoTimerElapsed, MistakesDuringSlowmo) * AccuracyModifier);

	// Set shot info
	ResolvedShotInfo.bSuccess = bShotSuccessful;
	ResolvedShotInfo.Accuracy = Accuracy;
	ResolvedShotInfo.MistakesMade = MistakesDuringSlowmo;
	ResolvedShotInfo.TimeTaken = SlowmoTimerElapsed;
	
	// Notify!
	if (ShotResultDelegate.IsBound())
	{
		ShotResultDelegate.Execute(ResolvedShotInfo);
	}

	// Remove all words
	for (TWeakObjectPtr<UWordWidget> Widget : WordWidgets)
	{
		Widget->RemoveFromParent();
	}
	WordWidgets.Empty();

	// Remove accuracy overlay
	if (AccuracyOverlay.IsValid())
	{
		AccuracyOverlay->SetFinalAccuracy(Accuracy);
		AccuracyOverlay.Reset();
	}

	// End the slowmo
	EndSlowmo();
}

void ASoccerPlayerController::BeginSlowmo(ASoccerPlayer* FromPlayer)
{
	MistakesDuringSlowmo = 0;
	SlowmoTimerElapsed = 0.0f;

	UE_LOG(LogTemp, Warning, TEXT("SLOWMO FOR %s"), *FromPlayer->GetName());

	AActor* Camera = PlayerCameraManager->GetViewTarget();
	GOOSE_BAIL(Camera);

	FVector CameraLocation = Camera->GetActorLocation();

	FQuat CameraQuat = Camera->GetActorQuat();
	FQuat CameraTargetQuat = UKismetMathLibrary::FindLookAtRotation(CameraLocation, FromPlayer->GetActorLocation()).Quaternion();
	
	RunGooseTween
	(
		GT_Parallel
		(
			GT_Ease(QuadraticOut, GT_Lambda(1.0f, PlayerCameraManager->GetFOVAngle(), 30.0f, [=](float Value)
			{
				PlayerCameraManager->SetFOV(Value);
			})),
			GT_Ease(QuadraticOut, GT_Lambda(1.0f, 0.0f, 1.0f, [=](float Value)
			{
				FQuat CurrentQuat = FQuat::Slerp(CameraQuat, CameraTargetQuat, Value);
				Camera->SetActorRotation(CurrentQuat);
			})),
			GT_Ease(QuadraticOut, GT_Lambda(1.0f, 1.0f, 0.001f, [=](float Value)
			{
				USlowableDownableComponent::SetTimeDilation(Value);
			}))
		)
	);

	bSlowmoActive = true;
}

void ASoccerPlayerController::EndSlowmo()
{
	AActor* Camera = PlayerCameraManager->GetViewTarget(); GOOSE_BAIL(Camera);

	FQuat CameraQuat = Camera->GetActorQuat();
	FQuat CameraTargetQuat = FRotator(-50.0f, -90.0f, 0.0f).Quaternion();

	RunGooseTween
	(
		GT_Parallel
		(
			GT_Ease(QuadraticIn, GT_Lambda(0.4f, PlayerCameraManager->GetFOVAngle(), 90.0f, [=](float Value)
			{
				PlayerCameraManager->SetFOV(Value);
			})),
			GT_Ease(QuadraticIn, GT_Lambda(0.4f, 0.0f, 1.0f, [=](float Value)
			{
				FQuat CurrentQuat = FQuat::Slerp(CameraQuat, CameraTargetQuat, Value);
				Camera->SetActorRotation(CurrentQuat);
			}))
		)
	);
	RunGooseTween
	(
		GT_Ease(QuadraticIn, GT_Lambda(2.0f, 0.001f, 1.0f, [=](float Value)
		{
			USlowableDownableComponent::SetTimeDilation(Value);
		}))
	);

	bSlowmoActive = false;
}

void ASoccerPlayerController::AddWordsToScreen(float TotalDifficulty)
{
	UWordsDatabase* DB = UWordsDatabase::GetMainWordsDatabase();
	GOOSE_BAIL(DB);
	
	float Difficulty = 0.0f;

	int32 SizeX, SizeY;
	GetViewportSize(SizeX, SizeY);

	TSet<FString> WordsUsed;

	while (Difficulty < TotalDifficulty)
	{
		const FWordEntry& ChosenWord = DB->Words[FMath::RandRange(0, DB->Words.Num() - 1)];
		if (WordsUsed.Contains(ChosenWord.Word))
		{
			continue;
		}

		WordsUsed.Add(ChosenWord.Word);

		FVector2D RandomPosition = FVector2D(FMath::RandRange(0.1f, 0.7f) * SizeX, FMath::RandRange(0.1f, 0.7f) * SizeY);

		UFullWordWidget* WordWidget = CreateWidget<UFullWordWidget>(this, UMG_FullWordWidget);
		WordWidget->AddToPlayerScreen();
		WordWidget->SetPositionInViewport(RandomPosition);

		WordWidget->InitWithWord(ChosenWord.Word, ChosenWord.Difficulty);

		Difficulty += ChosenWord.Difficulty;
	}
}

ADecalActor* ASoccerPlayerController::AddArrow(FVector From, FVector To, FLinearColor Colour) const
{
	// Add arrow decal
	FVector MidPoint = (From + To) / 2.0f;
	MidPoint.Z = 290.0f;

	float Distance = FVector::Dist(From, To);
	FRotator DecalRotation = UKismetMathLibrary::FindLookAtRotation(From, To);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ADecalActor* Decal = GetWorld()->SpawnActor<ADecalActor>(MidPoint, DecalRotation, SpawnParams);
	if (GOOSE_VERIFY(Decal))
	{
		UDecalComponent* DecalComp = Decal->GetDecal();

		DecalComp->DecalSize = FVector(128.0f, Distance / 4.0f, Distance / 2.0f);
		DecalComp->FadeStartDelay = MAX_SLOWMO_TIME - 1.0f;
		DecalComp->FadeDuration = 2.0f;

		// Create material
		static const FName NAME_Colour = TEXT("Colour");

		UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(M_Arrow, Decal);
		MID->SetVectorParameterValue(NAME_Colour, Colour);
		DecalComp->SetDecalMaterial(MID);
	}

	return Decal;
}

void ASoccerPlayerController::RequestExit()
{
	UGameplayStatics::OpenLevel(this, TEXT("TitleScreen"));
}

float ASoccerPlayerController::CalculateAccuracy(float TimeTaken, int32 MistakesMade) const
{
	float Accuracy = 1.0f - FMath::Pow(TimeTaken / MAX_SLOWMO_TIME, 2.0f);
	Accuracy -= 0.1f * MistakesMade;

	return FMath::Max(Accuracy, 0.0f);
}