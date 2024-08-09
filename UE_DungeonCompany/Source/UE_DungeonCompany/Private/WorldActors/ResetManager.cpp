// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/ResetManager.h"


// Sets default values
AResetManager::AResetManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	this->bReplicates=true;
}

// Called when the game starts or when spawned
void AResetManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AResetManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AResetManager::BP_ResetDungeon_Implementation()
{
}

void AResetManager::Server_ResetDungeon_Implementation()
{
	BP_ResetDungeon();
}

