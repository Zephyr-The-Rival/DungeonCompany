// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallGenerator.generated.h"

UCLASS()
class UE_DUNGEONCOMPANY_API AWallGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWallGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION(BlueprintCallable)
	void SpawnBricks(TArray<UStaticMeshComponent*> Bricks, FString NameSuffix);
};
