// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LostItemsCollection.generated.h"

class UItemData;

USTRUCT(Blueprintable)
struct FLostItem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UItemData> ItemClass;

	UPROPERTY(BlueprintReadWrite)
	FString ItemData;
};

UCLASS()
class UE_DUNGEONCOMPANY_API ALostItemsCollection : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FLostItem> LostItems;

public:
	ALostItemsCollection();

	UFUNCTION(BlueprintCallable)
	void AddLostItem(UItemData* InLostItem);
};
