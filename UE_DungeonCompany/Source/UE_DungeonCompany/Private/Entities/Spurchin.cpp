// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Spurchin.h"

#include "Perception/AISenseConfig_Sight.h"


void ASpurchin::HandleSightSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent)
{
	if(!bSightEnabled)
		return;

	if(!Stimulus.WasSuccessfullySensed())
		bSightEnabled = false;

	Super::HandleSightSense(Actor, Stimulus, BlackboardComponent);
}

void ASpurchin::HandleHearingSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent)
{
	Super::HandleHearingSense(Actor, Stimulus, BlackboardComponent);

	bSightEnabled = true;
}
