// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/TimedInteractionPuzzle/PuzzleButton.h"

APuzzleButton::APuzzleButton()
{
	PrimaryActorTick.bCanEverTick = false;

	bInteractOnServer = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;

}

void APuzzleButton::BeginPlay()
{
	Super::BeginPlay();

	DefaultMaterial = StaticMesh->GetMaterial(0);
	
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

void APuzzleButton::OnTriggerStarted_Implementation()
{
	StaticMesh->SetMaterial(0, ActivatedMaterial);

}

void APuzzleButton::OnTriggerStopped_Implementation()
{
	StaticMesh->SetMaterial(0, DefaultMaterial);

}

