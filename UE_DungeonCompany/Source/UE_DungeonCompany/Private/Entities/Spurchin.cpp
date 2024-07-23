// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Spurchin.h"

#include "Perception/AISenseConfig_Sight.h"


void ASpurchin::HandleSightSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent)
{
	if (!Stimulus.WasSuccessfullySensed())
	{
		GetSenseConfig<UAISenseConfig_Sight>()->SightRadius = 0.f;
		UpdatePerception();
		Super::HandleSightSense(Actor, Stimulus, BlackboardComponent);
	}

}

void ASpurchin::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void ASpurchin::OnTargetingPlayer_Implementation(APlayerCharacter* Target)
{
	if(!Target)
		return;

	GetSenseConfig<UAISenseConfig_Sight>()->SightRadius = OriginalSightRadius;
	UpdatePerception();
}

void ASpurchin::BeginPlay()
{
	Super::BeginPlay();

	OriginalSightRadius = GetSenseConfig<UAISenseConfig_Sight>()->SightRadius;
	GetSenseConfig<UAISenseConfig_Sight>()->SightRadius = 0.f;

	UpdatePerception();
}

void ASpurchin::HandleHearingSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent)
{
	Super::HandleHearingSense(Actor, Stimulus, BlackboardComponent);
}
