// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "SoccerPlayer.h"

//Engine
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"

// AI
#include "AIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DetourCrowdAIController.h"

// Paper2D
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"

// Game
#include "SoccerBall.h"
#include "Components/SlowableDownableComponent.h"
#include "System/SoccerAIController.h"
#include "System/SoccerPlayerController.h"

namespace
{
	const FName NAME_NumPasses = TEXT("NumPasses");
}

ASoccerPlayer::ASoccerPlayer()
	: DribbleTimer(0.0f)
	, bIsDribbling(false)
{
	PrimaryActorTick.bCanEverTick = true;

	// Resources
	BT_SoccerPlayer = FIND_RESOURCE(BehaviorTree, BT_SoccerPlayer, TEXT("AI"));
	T_Player_Flipbook = FIND_RESOURCE(PaperFlipbook, T_Player_Flipbook, TEXT("Textures"));
	SND_Kick = FIND_RESOURCE(SoundWave, Kick, TEXT("Sounds"));
	//

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ASoccerAIController::StaticClass();

	// Set up character stuff:
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCapsuleHalfHeight(120.0f);
	Capsule->SetCapsuleRadius(68.0f);
	
	UCharacterMovementComponent* CharMovement = GetCharacterMovement();
	CharMovement->bOrientRotationToMovement = true;
	CharMovement->RotationRate = FRotator(0.1f);
	
	// Set up slowable downable
	SlowableComponent = CreateDefaultSubobject<USlowableDownableComponent>(TEXT("Slowable"));

	// Set up sprite
	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PlayerFlipbook"));
	FlipbookComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, -45.0f));
	FlipbookComponent->bAbsoluteRotation = true;
	FlipbookComponent->bReceivesDecals = false;
	FlipbookComponent->SetFlipbook(T_Player_Flipbook);
	FlipbookComponent->SetupAttachment(Capsule);	
}

void ASoccerPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	AAIController* AIController = GetController<AAIController>();									
	GOOSE_BAIL(AIController);

	AIController->RunBehaviorTree(BT_SoccerPlayer);

	// Assign team based on half of pitch ;)
	if (GetActorLocation().X < 0.0f)
	{
		SetTeam(ETeam::TEAM_Red);
	}
	else
	{
		SetTeam(ETeam::TEAM_Blue);
	}	

	// Remember start location
	UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();						
	GOOSE_BAIL(Blackboard);

	static const FName Name_StartLocation = TEXT("StartLocation");
	Blackboard->SetValueAsVector(Name_StartLocation, GetActorLocation());
}

void ASoccerPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DribbleTimer > 0.0f)
	{
		DribbleTimer -= DeltaTime;
	}
}

void ASoccerPlayer::SetTeam(ETeam InTeam)
{
	Team = InTeam;

	// Update flipbook
	FlipbookComponent->SetSpriteColor(ASoccerPlayer::GetColourForTeam(Team));

	// Update blackboard
	AAIController* AIController = GetController<AAIController>();									
	GOOSE_BAIL(AIController);

	UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();						
	GOOSE_BAIL(Blackboard);

	static const FName NAME_IsBlue = TEXT("bIsBlue");
	Blackboard->SetValueAsBool(NAME_IsBlue, InTeam == ETeam::TEAM_Blue);
}
ETeam ASoccerPlayer::GetTeam() const
{
	return Team;
}

void ASoccerPlayer::NotifyDribbling()
{
	DribbleTimer = 2.0f; // No dribble for 2 secs!
	bIsDribbling = true;

	// Set random number of passes before shooting
	SetNumberOfPasses(FMath::RandRange(0, 1));
}
void ASoccerPlayer::NotifyNoLongerDribbling()
{
	bIsDribbling = false;
}
bool ASoccerPlayer::IsDribbling() const
{
	return bIsDribbling;
}
bool ASoccerPlayer::IsEligibleForDribble() const
{
	return DribbleTimer <= 0.0f;
}

void ASoccerPlayer::Pass()
{
	if (!GOOSE_VERIFY(bIsDribbling))
	{
		return;
	}
	
	// Find other players on the same team
	TArray<ASoccerPlayer*> ThisTeam;

	// Get all players in the world
	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	for (TActorIterator<ASoccerPlayer> PlayerIt(World); PlayerIt; ++PlayerIt)
	{
		ASoccerPlayer* Player = *PlayerIt;															
		GOOSE_BAIL_CONTINUE(Player);

		if (Player->GetTeam() == Team)
		{
			ThisTeam.Add(Player);
		}
	}

	// Sort by who is most back
	ThisTeam.Sort([this](const ASoccerPlayer& LHS, const ASoccerPlayer& RHS)
	{
		float ThisLoc = GetActorLocation().X;

		if (Team == ETeam::TEAM_Red)
		{
			return (LHS.GetActorLocation().X - ThisLoc) < (RHS.GetActorLocation().X - ThisLoc);
		}
		else
		{
			return (LHS.GetActorLocation().X - ThisLoc) > (RHS.GetActorLocation().X - ThisLoc);
		}		
	});

	FColor ThisColour = Team == ETeam::TEAM_Red ? FColor::Red : FColor::Blue;

	// Default to first, unless first is us!
	ASoccerPlayer* TargetPlayer = ThisTeam[0];
	if (TargetPlayer == this && ThisTeam.Num() > 1)
	{
		TargetPlayer = ThisTeam[1];
	}
	
	// Work out the one "nearest behind"
	float PrevVal = 0.0f;
	for (ASoccerPlayer* Player : ThisTeam)
	{
		float Val = GetActorLocation().X - Player->GetActorLocation().X;
		if (Val >= 0.0f)
		{
			TargetPlayer = Player;
			break;
		}

		if (Player != this)
		{
			PrevVal = Val;
		}
	}

	// Boot towards goal
	FVector GoalTarget = TargetPlayer->GetActorLocation();
	FVector Impulse = (GoalTarget - GetActorLocation()).GetSafeNormal() * 20'00.0f; // Scale impulse by distance?

	ASoccerPlayerController* PC = Cast<ASoccerPlayerController>(GetWorld()->GetFirstPlayerController());
	GOOSE_BAIL(PC);

	FShotTargetInfo Info;
	Info.Shooter = this;
	Info.Target = GoalTarget;
	
	FShotResultDelegate Delegate;
	Delegate.BindLambda([=](FResolvedShotInfo ShotInfo)
	{
		KickBall(Impulse, ShotInfo.Accuracy);
	});

	PC->PauseForInput(Info, TEXT("Pass"), Delegate);
}
void ASoccerPlayer::Shoot()
{
	if (!GOOSE_VERIFY(bIsDribbling))
	{
		return;
	}

	FVector GoalTarget = FVector(Team == ETeam::TEAM_Blue ? -18'50.0f : +18'50.0f, 0.0f, GetActorLocation().Z);
	FVector Impulse = (GoalTarget - GetActorLocation()).GetSafeNormal() * 50'00.0f;

	ASoccerPlayerController* PC = Cast<ASoccerPlayerController>(GetWorld()->GetFirstPlayerController());
	GOOSE_BAIL(PC);

	FShotTargetInfo Info;
	Info.Shooter = this;
	Info.Target = GoalTarget;

	FShotResultDelegate Delegate;
	Delegate.BindLambda([=](FResolvedShotInfo ShotInfo)
	{
		KickBall(Impulse, ShotInfo.Accuracy);
	});

	PC->PauseForInput(Info, TEXT("Shoot"), Delegate);
}

FLinearColor ASoccerPlayer::GetColourForTeam(ETeam Team)
{
	switch (Team)
	{
	case ETeam::TEAM_Red:
		return FLinearColor::Red;

	case ETeam::TEAM_Blue:
		return FLinearColor::Blue;
	}

	return FLinearColor::White;
}

int32 ASoccerPlayer::GetNumberOfPasses() const
{
	UBlackboardComponent* Blackboard = GetBlackboard();												
	GOOSE_BAIL_RETURN(Blackboard, 0);

	return Blackboard->GetValueAsInt(NAME_NumPasses);
}
void ASoccerPlayer::SetNumberOfPasses(int32 PassesBeforeShooting)
{
	UBlackboardComponent* Blackboard = GetBlackboard();												
	GOOSE_BAIL(Blackboard);

	Blackboard->SetValueAsInt(NAME_NumPasses, PassesBeforeShooting);
}

bool ASoccerPlayer::KickBall(FVector KickImpulse, float Accuracy)
{
	UWorld* World = GetWorld();																		
	GOOSE_BAIL_RETURN(World, false);
	
	TActorIterator<ASoccerBall> It(World);															
	GOOSE_BAIL_RETURN(It, false);
	ASoccerBall* Ball = *It;																		
	GOOSE_BAIL_RETURN(Ball, false);

	// Detach us as dribbler
	Ball->DetachDribbler();
	
	// Move to sensible location
	FVector SensibleLocation = GetActorLocation() + KickImpulse.GetSafeNormal() * 50.0f;
	Ball->SetActorLocation(SensibleLocation);

	// Apply accuracy
	{
		constexpr float MAX_OFFSET = 50'00.0f; // 50m

		FVector KickFwd = KickImpulse.GetSafeNormal();
		FVector KickRight = FVector::CrossProduct(KickFwd, FVector::UpVector);
		FVector KickUp = FVector::CrossProduct(KickRight, KickFwd);

		float OffsetMagnitude = MAX_OFFSET * (1.0f - Accuracy);
		float AngleRadians = FMath::FRandRange(0.0f, 2 * PI);

		KickImpulse += KickRight * FMath::Sin(AngleRadians) * OffsetMagnitude;
		KickImpulse += KickUp * FMath::Cos(AngleRadians) * OffsetMagnitude;
	}

	// Get the physical component of the ball and hit it
	UPrimitiveComponent* PrimComp = Ball->FindComponentByClass<UPrimitiveComponent>();				
	GOOSE_BAIL_RETURN(PrimComp, false);

	PrimComp->AddImpulse(KickImpulse, NAME_None, true);

	// Play kicking sound
	UGameplayStatics::PlaySoundAtLocation(this, SND_Kick, GetActorLocation(), FMath::Min(0.8f, Accuracy), FMath::RandRange(0.8f, 1.2f));

	return true;
}

UBlackboardComponent* ASoccerPlayer::GetBlackboard() const
{
	AAIController* AIController = GetController<AAIController>();									
	GOOSE_BAIL_RETURN(AIController, nullptr);

	UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();						
	GOOSE_BAIL_RETURN(Blackboard, nullptr);

	return Blackboard;
}