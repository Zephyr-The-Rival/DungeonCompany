// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "DC_AIController.generated.h"

/**
 * 
 */
 class UAISenseConfig_Sight;
 class UAISenseConfig_Hearing;

UCLASS()
class UE_DUNGEONCOMPANY_API ADC_AIController : public AAIController
{
	GENERATED_BODY()

private:
	UAISenseConfig_Sight* SightConfig;
	UAISenseConfig_Hearing* HearingConfig;

public:
	explicit ADC_AIController(FObjectInitializer const& ObjectInitializer);

protected:
	virtual void OnPossess(APawn* InPawn) override;

	void SetupPerceptionSystem();

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus const Stimulus);

	void HandleSightSense(AActor* Actor, FAIStimulus const Stimulus);
	void HandleHearingSense(FAIStimulus const Stimulus);
	
};
