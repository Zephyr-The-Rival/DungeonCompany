// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "WorldCurrency_Data.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UWorldCurrency_Data : public UItemData
{
	GENERATED_BODY()

	
public:
	UPROPERTY(BlueprintReadWrite)
	int32 PickUpValue=1;

private:

	void DeserializeMyData_Implementation(const FString& StringData) override;
	FString SerializeMyData_Implementation() override;

};
