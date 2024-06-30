// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "QuasoSnakeSpawnVolume.generated.h"

class AQuasoSnake;

/**
 * 
 */
UCLASS(Blueprintable)
class UE_DUNGEONCOMPANY_API AQuasoSnakeSpawnVolume : public ATriggerVolume
{
	GENERATED_BODY()

private:
	TSubclassOf<AQuasoSnake> QuasoSnakeClass;

	UPROPERTY(EditAnywhere, Category = "Balancing")
	float MinSpawnRadiusAroundPlayer = 200.f;

	UPROPERTY(EditAnywhere, Category = "Balancing")
	float MaxSpawnRadiusAroundPlayer = 500.f;

	UPROPERTY(EditAnywhere, Category = "Balancing")
	float DespawnSeconds = 30.f;

	UPROPERTY(EditAnywhere, Category = "Balancing")
	float RespawnSeconds = 60.f;

public:
	AQuasoSnakeSpawnVolume();

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle DespawnHandle;
	bool bDespawnTimerRunning = false;

	FTimerHandle RespawnHandle;

	bool bSpawnedQuasoSnake = false;
	bool bQuasoSnakeReturned = false;

protected:
	UFUNCTION()
	void ReturnQuasoSnake();

	UFUNCTION()
	void EndQuasoSnake(AActor* Actor, EEndPlayReason::Type EndPlayReason);
	
public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

private:
	ACharacter* PlayerSpawningCloseTo;
	AQuasoSnake* SpawnedQuasoSnake;

public:
	void SpawnCloseToPlayer(ACharacter* Character);
	void DespawnQuasoSnake();
};
