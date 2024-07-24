// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "DC_AIController.generated.h"

/**
 * 
 */
class AAIEntity;

 class UAISenseConfig_Sight;
 class UAISenseConfig_Hearing;

UCLASS()
class UE_DUNGEONCOMPANY_API ADC_AIController : public AAIController
{
	GENERATED_BODY()

private:
	bool bUsingBlackboard = false;

	AAIEntity* AIEntity;

public:
	explicit ADC_AIController(FObjectInitializer const& ObjectInitializer);

protected:
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus const Stimulus);

public:
	void PutAggroOnPlayer(APlayerCharacter* AggroPullingPlayer);

	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetingPlayer, APlayerCharacter*, TargetPlayer);

	FOnTargetingPlayer OnTargetingPlayer;
	
};
