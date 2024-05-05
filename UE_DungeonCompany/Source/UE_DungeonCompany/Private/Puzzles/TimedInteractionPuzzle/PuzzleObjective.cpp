// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/TimedInteractionPuzzle/PuzzleObjective.h"
#include "Puzzles/TimedInteractionPuzzle/PuzzleTrigger.h"

#include "EngineUtils.h"


APuzzleObjective::APuzzleObjective()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

}

void APuzzleObjective::BeginPlay()
{
	Super::BeginPlay();

	bool incrementMinActivatedTriggers = !MinActivatedTriggers;

	for (TActorIterator<APuzzleTrigger> It(GetWorld()); It; ++It)
	{
		uint8 channel = It->GetChannel();

		if(channel != Channel)
			continue;

		It->OnTriggered.AddDynamic(this, &APuzzleObjective::OnPuzzleTriggerTriggered);
		MinActivatedTriggers += incrementMinActivatedTriggers;
		
	}
	
}

void APuzzleObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APuzzleObjective::OnPuzzleTriggerTriggered(APuzzleTrigger* TriggeredPuzzleTrigger)
{
	if (ActivatedPuzzleTriggers.Contains(TriggeredPuzzleTrigger))
		return;

	ActivatedPuzzleTriggers.Add(TriggeredPuzzleTrigger);

	FTimerDelegate delegate = FTimerDelegate::CreateLambda([this, TriggeredPuzzleTrigger]() {
		ActivatedPuzzleTriggers.Remove(TriggeredPuzzleTrigger);
		});

	FTimerHandle timerHandle;

	GetWorld()->GetTimerManager().SetTimer(timerHandle, delegate, TimingTolerance, false);
	CheckTriggerCount();
}

void APuzzleObjective::CheckTriggerCount()
{
	if (ActivatedPuzzleTriggers.Num() < MinActivatedTriggers)
		return;

	if (HasAuthority())
		OnTriggerNumReached();
}

void APuzzleObjective::OnTriggerNumReached_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "All buttons pressed!");
}

