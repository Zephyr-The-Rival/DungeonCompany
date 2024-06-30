// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemData.h"

FString UItemData::SerializeMyData_Implementation()
{
	return FString();
}

void UItemData::DeserializeMyData_Implementation(const FString& StringData)
{

}

TArray<FString> UItemData::SeperateStringData(const FString& StringData)
{
	TArray<FString> seperatedStrings = TArray<FString>();
	seperatedStrings.Add("");


	int i = 0;
	int seperateNum = 0;
	int seperateIterator = 0;
	while (StringData.IsValidIndex(i))
	{
		if (StringData[i] == ';')
		{
			seperateNum++;
			seperatedStrings.Add("");

		}
		else
		{
			seperatedStrings[seperateNum] += StringData[i];
			seperateIterator = 0;

		}
		i++;
		seperateIterator++;
	}
	return seperatedStrings;
}
