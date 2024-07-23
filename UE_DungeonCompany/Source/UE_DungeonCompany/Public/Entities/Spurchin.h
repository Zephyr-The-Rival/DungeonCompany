// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/AIEntity.h"
#include "Spurchin.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API ASpurchin : public AAIEntity
{
	GENERATED_BODY()

private:
	bool bSightEnabled = false;

protected:
	virtual void HandleSightSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent);
	virtual void HandleHearingSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent);
	
};
