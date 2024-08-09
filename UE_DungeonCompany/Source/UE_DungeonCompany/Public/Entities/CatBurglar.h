// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/AIEntity.h"
#include "CatBurglar.generated.h"

/**
 * 
 */

UENUM()
enum class ECatBurglarBehaviorState : uint8 {
	Stalking = 0 UMETA(DisplayName = "Stalking"),
	Prowling = 1 UMETA(DisplayName = "Prowling"),
	Harassing = 2 UMETA(DisplayName = "Harassing"),
	Fleeing = 3 UMETA(DisplayName = "Fleeing"),
	Retrieving= 4 UMETA(DisplayName = "Retrieving")
};

class AWorldItem;
class UItemData;

UCLASS()
class UE_DUNGEONCOMPANY_API ACatBurglar : public AAIEntity
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere)
	float ItemDetectionRadius = 500.f;

	UPROPERTY(EditAnywhere)
	TMap<ECatBurglarBehaviorState, UBehaviorTree*> BehaviorTreesForStates;

private:
	ECatBurglarBehaviorState IdleBehaviorState;
	ECatBurglarBehaviorState CurrentBehaviorState;
	
	TArray<AWorldItem*> WorldItemsInSight;
	UItemData* StolenItem;

public:
	void SetIdleBehaviorState(ECatBurglarBehaviorState NewIdleState);

public:
	ACatBurglar();

public:
	void StealItem(AWorldItem* StealingItem);

protected:
	void UpdateBehavior(ECatBurglarBehaviorState NewBehaviorState);

protected:
	virtual void OnPlayerAttackHit(APlayerCharacter* PlayerCharacter) override;

protected:
	virtual void HandleSightSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent) override;
	virtual void HandleHearingSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent) override;
	
};
