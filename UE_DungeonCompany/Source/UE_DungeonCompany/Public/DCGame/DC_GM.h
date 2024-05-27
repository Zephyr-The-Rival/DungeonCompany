// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DC_GM.generated.h"

/**
 * 
 */
 class AAIEntity;

UCLASS()
class UE_DUNGEONCOMPANY_API ADC_GM : public AGameMode
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AAIEntity>> AIClasses;

public:
	ADC_GM();

protected:
	virtual void BeginPlay() override;

public:
	void Respawn(AController* Controller);

	/* If you want to Spawn the AI randomly on the map then leave the NearActor param on null.
	In this case the Radius parameter can be ignored */
	AAIEntity* SpawnAIEntity(UClass* Class, AActor* NearActor = nullptr, float Radius = 500.f);

protected:
	template<class T = AAIEntity>
	T* RandomlySpawnAIEntity(UClass* Class) const;

	template<class T = AAIEntity>
	T* SpawnAIEntityCloseToActor(UClass* Class, AActor* Actor, float Radius) const;

	template<class T>
	T* SpawnAIEntity(UClass* Class, FVector Location, FRotator Rotation = FRotator::ZeroRotator) const;

};
