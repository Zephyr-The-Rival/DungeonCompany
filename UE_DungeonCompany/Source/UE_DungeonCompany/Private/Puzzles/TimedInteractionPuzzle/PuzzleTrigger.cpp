// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/TimedInteractionPuzzle/PuzzleTrigger.h"

// Sets default values
APuzzleTrigger::APuzzleTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

}

// Called when the game starts or when spawned
void APuzzleTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APuzzleTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APuzzleTrigger::Trigger()
{
	OnTriggered.Broadcast(this);
}

