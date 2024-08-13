// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/SaveManager.h"

#include "Items/WorldItem.h"


// Sets default values
ASaveManager::ASaveManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

}

// Called when the game starts or when spawned
void ASaveManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASaveManager::SpawnWorldItem(TSubclassOf<AWorldItem> ItemClass ,FTransform Transform ,FString SerializedData)
{
	AWorldItem* i = GetWorld()->SpawnActorDeferred<AWorldItem>(ItemClass, Transform);
	i->SerializedStringData = SerializedData;
	i->FinishSpawning(Transform);
}


