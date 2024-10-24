// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/SharedStatsManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASharedStatsManager::ASharedStatsManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->bReplicates = true;
}

// Called when the game starts or when spawned
void ASharedStatsManager::BeginPlay()
{
	Super::BeginPlay();
	
}


void ASharedStatsManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASharedStatsManager, Money);
}