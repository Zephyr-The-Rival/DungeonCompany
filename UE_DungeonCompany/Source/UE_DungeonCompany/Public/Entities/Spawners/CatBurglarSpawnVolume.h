// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "CatBurglarSpawnVolume.generated.h"

class ACatBurglar;
class APlayerCharacter;
class ADC_Entity;
/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API ACatBurglarSpawnVolume : public ATriggerVolume
{
	GENERATED_UCLASS_BODY()

private:
	UPROPERTY(EditInstanceOnly, meta = (MakeEditWidget = true))
	FVector SpawnLocation;

	UPROPERTY(EditAnywhere)
	float MinSpawnDistance = 1500.f;

	UPROPERTY(EditAnywhere)
	uint8 MaxCatBurglar = 5;

	UPROPERTY(EditAnywhere)
	float VolumeLeftBurglarDespawnDelay = 30.f;

	TSubclassOf<ACatBurglar> CatBurglarClass;

private:
	TArray<APlayerCharacter*> PlayerCharactersInVolume;
	TArray<ACatBurglar*> SpawnedCatBurglars;

	FTimerHandle DespawnBurglarHandle;

	bool bBurglarsCanBeDespawned = false;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:
	void MarkBurglarsAsDespawnable();

	void DespawnIdleBurglars();
	void DespawnBurglarIfOnIdle(ACatBurglar* InCatBurglar);

private:
	UPROPERTY(EditAnywhere)
	float AllowSpawnDelay = 60.f;

	FTimerHandle AllowSpawnHandle;

public:
	inline FVector GetWorldSpawnLocation() const { return GetActorLocation() + SpawnLocation; }
	ACatBurglar* SpawnCatBurglarWithPlayerAsTarget(APlayerCharacter* TargetPlayer);

protected:
	UFUNCTION()
	void OnCatBurglarDeath(ADC_Entity* DeadBurglar);

	void OnCatBurglarCountChanged();
};
