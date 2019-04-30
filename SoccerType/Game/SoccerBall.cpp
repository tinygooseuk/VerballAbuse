// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "SoccerBall.h"

// Engine
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Game
#include "Components/SlowableDownableComponent.h"
#include "Game/SoccerPlayer.h"

ASoccerBall::ASoccerBall()
	: Dribbler(nullptr)
	, DribbleDebounceTimer(0.0f)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	// Resources
	UStaticMesh* SM_SoccerBall = FIND_RESOURCE(StaticMesh, SM_SoccerBall, TEXT("StaticMeshes"));
	//

	SlowableComponent = CreateDefaultSubobject<USlowableDownableComponent>(TEXT("Slowable"));

	BallMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	BallMeshComponent->SetStaticMesh(SM_SoccerBall);
	BallMeshComponent->SetSimulatePhysics(true);
	BallMeshComponent->SetWorldScale3D(FVector(2.0f));
	SetRootComponent(BallMeshComponent);

	BallDribbleDetectorComponent = CreateDefaultSubobject<USphereComponent>(TEXT("BallDribbleDetector"));
	BallDribbleDetectorComponent->bAbsoluteScale = true;
	BallDribbleDetectorComponent->InitSphereRadius(50.0f);
	BallDribbleDetectorComponent->SetGenerateOverlapEvents(true);
	BallDribbleDetectorComponent->OnComponentBeginOverlap.AddDynamic(this, &ASoccerBall::OnHit);
	BallDribbleDetectorComponent->SetupAttachment(BallMeshComponent);
}

void ASoccerBall::BeginPlay()
{
	Super::BeginPlay();
	
	BallMeshComponent->SetUseCCD(true);
}

void ASoccerBall::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (DribbleDebounceTimer > 0.0f)
	{
		DribbleDebounceTimer -= DeltaSeconds;
	}
}

void ASoccerBall::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASoccerPlayer* Player = Cast<ASoccerPlayer>(OtherActor);

	if (!Player)
	{
		return;
	}

	AttachDribbler(Player);
}

void ASoccerBall::AttachDribbler(ASoccerPlayer* Player)
{
	if (!Player || !Player->IsEligibleForDribble() || DribbleDebounceTimer > 0.0f)
	{
		// No player or player can't dribble again yet
		return;
	}

	if (Dribbler.IsValid())
	{
		if (Dribbler == Player)
		{
			// Already dribbling!
			return;
		}
		else
		{
			// Taking from another player, detach them first!
			DetachDribbler();
		}
	}

	Dribbler = Player;

	BallMeshComponent->SetWorldLocation(Player->GetActorLocation() + FVector(50.0f, -20.0f, -120.0f));
	BallMeshComponent->SetSimulatePhysics(false);
	BallMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BallMeshComponent->AttachToComponent(Player->GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);	

	// Notify player
	Player->NotifyDribbling();

	// Set timer 
	DribbleDebounceTimer = 2.0f;
}
void ASoccerBall::DetachDribbler()
{
	if (!Dribbler.IsValid())
	{
		return;
	}

	// Notify player
	Dribbler->NotifyNoLongerDribbling();

	// Remove from dribbler
	BallMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Dribbler = nullptr;

	// Re-enable physics
	BallMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	BallMeshComponent->SetSimulatePhysics(true);
}

ASoccerPlayer* ASoccerBall::GetDribbler() const
{
	return Dribbler.IsValid() ? Dribbler.Get() : nullptr;
}