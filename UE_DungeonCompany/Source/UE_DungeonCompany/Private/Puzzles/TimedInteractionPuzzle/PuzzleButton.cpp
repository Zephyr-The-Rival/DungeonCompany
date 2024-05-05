// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/TimedInteractionPuzzle/PuzzleButton.h"

APuzzleButton::APuzzleButton()
{
	PrimaryActorTick.bCanEverTick = false;

	bInteractOnServer = true;

}

void APuzzleButton::BeginPlay()
{
	Super::BeginPlay();
	
}

void APuzzleButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APuzzleButton::Interact(APawn* InteractingPawn)
{

}

void APuzzleButton::AuthorityInteract(APawn* InteractingPawn)
{
	Trigger();

}

