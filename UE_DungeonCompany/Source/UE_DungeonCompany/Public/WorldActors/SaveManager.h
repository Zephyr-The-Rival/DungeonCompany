// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SaveManager.generated.h"

class AWorldItem;

UCLASS()
class UE_DUNGEONCOMPANY_API ASaveManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASaveManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


protected:

	UFUNCTION(BlueprintCallable)
	void SpawnWorldItem(TSubclassOf<AWorldItem> ItemClass ,FTransform Transform ,FString SerializedData);
};
