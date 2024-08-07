// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/AIEntity.h"
#include "CatBurglar.generated.h"

/**
 * 
 */

class AWorldItem;

UCLASS()
class UE_DUNGEONCOMPANY_API ACatBurglar : public AAIEntity
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	float ItemDetectionRadius = 500.f;

	TArray<AWorldItem*> WorldItemsInSight;
	
protected:
	virtual void HandleSightSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent);
	virtual void HandleHearingSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent);
	
};
