// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/TimedInteractionPuzzle/PuzzleTrigger.h"

// Sets default values
APuzzleTrigger::APuzzleTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

}

void APuzzleTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void APuzzleTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APuzzleTrigger::Trigger()
{
	OnTriggered.Broadcast(this);
}

void APuzzleTrigger::Multicast_Triggered_Implementation()
{
	OnTriggerStarted();
}

void APuzzleTrigger::OnTriggerStarted_Implementation()
{

}

void APuzzleTrigger::Multicast_NotTriggered_Implementation()
{
	OnTriggerStopped();
}

void APuzzleTrigger::OnTriggerStopped_Implementation()
{

}

