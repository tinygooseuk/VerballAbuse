// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "SoccerType.h"
#include "Engine/DataAsset.h"
#include "WordsDatabase.generated.h"

USTRUCT()
struct FWordEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(UIMin=0.0, UIMax=1.0))
	float Difficulty;

	UPROPERTY(EditAnywhere)
	FString Word;
};

UCLASS()
class SOCCERTYPE_API UWordsDatabase : public UDataAsset
{
public:
	GENERATED_BODY()

	static UWordsDatabase* GetMainWordsDatabase();

	UPROPERTY(EditAnywhere)
	TArray<FWordEntry> Words;
};
