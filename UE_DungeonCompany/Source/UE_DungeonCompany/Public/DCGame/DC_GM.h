// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DC_GM.generated.h"

class APlayerCharacter;
class UItemData;
/**
 * 
 */
class AAIEntity;
class AWorldItem;

UCLASS()
class UE_DUNGEONCOMPANY_API ADC_GM : public AGameMode
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AAIEntity>> AIClasses;

	UPROPERTY(EditAnywhere)
	int SpurchinCount = 10.f;

public:
	ADC_GM();

protected:
	virtual void BeginPlay() override;
	void SpawnSpurchins();

public:
	void SpawnWorldItem(TSubclassOf<AWorldItem> ItemToSpawn, FTransform SpawnTransform, const FString& SerializedData);

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ADC_PostMortemPawn> PostMortemPawnClass;

public:
	void Respawn(AController* Controller);
	void StartSpectating(AController* Controller);

	/* If you want to Spawn the AI randomly on the map then leave the NearActor param on null.
	In this case the Radius parameter can be ignored */
	AAIEntity* SpawnAIEntity(UClass* Class, AActor* NearActor = nullptr, float Radius = 500.f);

protected:
	template <class T = AAIEntity>
	T* RandomlySpawnAIEntity(UClass* Class) const;

	template <class T = AAIEntity>
	T* SpawnAIEntityCloseToActor(UClass* Class, AActor* Actor, float Radius) const;

	template <class T>
	T* SpawnAIEntity(UClass* Class, FVector Location, FRotator Rotation = FRotator::ZeroRotator) const;
	
	
};
