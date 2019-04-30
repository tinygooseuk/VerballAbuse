// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "Particles/Emitter.h"
#include "ExplosionEmitter.generated.h"

UCLASS()
class SOCCERTYPE_API AExplosionEmitter : public AEmitter
{
	GENERATED_BODY()

public:
	AExplosionEmitter();

	static AExplosionEmitter* SpawnExplosion(UObject* WorldContextObject, FLinearColor Colour, FVector Location, FRotator Rotation, float Scale = 1.0f);

private:
	///////////////////////// Resources /////////////////////////
	UPROPERTY()
	class UParticleSystem* PS_Explosion;
};
