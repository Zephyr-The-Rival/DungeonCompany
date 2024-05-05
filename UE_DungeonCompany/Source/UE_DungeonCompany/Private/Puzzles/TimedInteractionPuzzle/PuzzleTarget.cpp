// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/TimedInteractionPuzzle/PuzzleTarget.h"
#include "Puzzles/TimedInteractionPuzzle/PuzzleTrigger.h"

#include "EngineUtils.h"


APuzzleTarget::APuzzleTarget()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

}

void APuzzleTarget::BeginPlay()
{
	Super::BeginPlay();

	bool incrementMinActivatedTriggers = !MinActivatedTriggers;

	for (TActorIterator<APuzzleTrigger> It(GetWorld()); It; ++It)
	{
		uint8 channel = It->GetChannel();

		if(channel != Channel)
			continue;

		It->OnTriggered.AddDynamic(this, &APuzzleTarget::OnPuzzleTriggerTriggered);
		MinActivatedTriggers += incrementMinActivatedTriggers;
		
	}
	
}

void APuzzleTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APuzzleTarget::OnPuzzleTriggerTriggered(APuzzleTrigger* TriggeredPuzzleTrigger)
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

void APuzzleTarget::CheckTriggerCount()
{
	if (ActivatedPuzzleTriggers.Num() < MinActivatedTriggers)
		return;

	if (HasAuthority())
		OnTriggerNumReached();
}

void APuzzleTarget::OnTriggerNumReached_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "All buttons pressed!");
}

