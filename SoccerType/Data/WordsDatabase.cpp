// (c) 2019 TinyGoose Ltd., All Rights Reserved.

#include "WordsDatabase.h"

UWordsDatabase* UWordsDatabase::GetMainWordsDatabase()
{
	UWordsDatabase* MainWords = UGooseUtil::GetObject<UWordsDatabase>(TEXT("MainWords"), TEXT("Data"));
	GOOSE_CHECK(MainWords);

	return MainWords;
}