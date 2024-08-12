// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "QuasoSnakeSpawnVolume.generated.h"

class AQuasoSnake;
class ADC_AIController;

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup=Spawner)
class UE_DUNGEONCOMPANY_API AQuasoSnakeSpawnVolume : public ATriggerVolume
{
	GENERATED_UCLASS_BODY()

private:
	TSubclassOf<AQuasoSnake> QuasoSnakeClass;

	UPROPERTY(EditAnywhere, Category = "Balancing|Spawn")
	float MinSpawnRadiusAroundPlayer = 200.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Spawn")
	float MaxSpawnRadiusAroundPlayer = 500.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Spawn")
	TArray<AActor*> SpawnsurfaceActors;

	UPROPERTY(EditAnywhere, Category = "Balancing|Spawn")
	float SpawnDistance = 20.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Timer")
	float OutOfRangeDespawnSeconds = 30.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Timer")
	float NotVisibleDespawnSeconds = 10.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Timer")
	float RespawnSeconds = 60.f;

public:

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle DespawnHandle;
	FTimerDelegate DespawnDelegate;
	bool bQuasoShouldBeDespawned = false;

	FTimerHandle RespawnHandle;
	bool bSpawnedQuasoSnake = false;
	bool bQuasoSnakeReturned = false;

	FTimerHandle NotVisibleHandle;
	FTimerDelegate NotVisibleDelegate;
	bool bPlayersCanSeeQuaso = false;

	bool bNotVisibleDespawn = false;

protected:
	UFUNCTION()
	void ReturnQuasoSnake();

	UFUNCTION()
	void EndQuasoSnake(AActor* Actor, EEndPlayReason::Type EndPlayReason);

private:
	ADC_AIController* QuasoSnakeController;
	bool bQuasoMovementStarted = false;
	
public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

private:
	ACharacter* PlayerSpawningCloseTo;
	AQuasoSnake* SpawnedQuasoSnake;

public:
	void SpawnCloseToPlayer(ACharacter* Character);
	
};
