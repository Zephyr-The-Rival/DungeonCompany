// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpurchinSpawner.generated.h"

class ASpurchin;

UCLASS(ClassGroup = Spawner)
class UE_DUNGEONCOMPANY_API ASpurchinSpawner : public AActor
{
	GENERATED_BODY()

private:
	TSubclassOf<ASpurchin> SpurchinClass;

	UPROPERTY(EditAnywhere)
	bool bForceSpawn = false;
	
public:
	inline bool IsSpawnForced() const { return bForceSpawn; }

public:	
	ASpurchinSpawner();

public:
	void SpawnSpurchin();

};
