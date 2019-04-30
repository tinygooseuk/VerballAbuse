// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "ExplosionEmitter.h"

// Engine
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

AExplosionEmitter::AExplosionEmitter()
{
	// Resources
	PS_Explosion = FIND_RESOURCE(ParticleSystem, PS_Explosion, TEXT("ParticleSystems"));
	//

	bDestroyOnSystemFinish = true;

	UParticleSystemComponent* ParticleSys = GetParticleSystemComponent();
	GOOSE_BAIL(ParticleSys);

	ParticleSys->SetTemplate(PS_Explosion);
}

AExplosionEmitter* AExplosionEmitter::SpawnExplosion(UObject* WorldContextObject, FLinearColor Colour, FVector Location, FRotator Rotation, float Scale)
{
	GOOSE_BAIL_RETURN(WorldContextObject, nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	GOOSE_BAIL_RETURN(World, nullptr);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AExplosionEmitter* Emitter = World->SpawnActor<AExplosionEmitter>(AExplosionEmitter::StaticClass(), Location, Rotation, SpawnParams);
	GOOSE_BAIL_RETURN(Emitter, nullptr);

	static const FName NAME_Colour = TEXT("Colour");
	Emitter->SetColorParameter(NAME_Colour, Colour);

	if (Scale != 1.0f)
	{
		Emitter->SetActorScale3D(FVector(Scale));
	}

	return Emitter;
}