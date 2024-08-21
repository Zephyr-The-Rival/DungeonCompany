// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/AIEntity.h"
#include "CatBurglar.generated.h"

/**
 * 
 */

UENUM()
enum class ECatBurglarBehaviorState : uint8
{
	Stalking = 0 UMETA(DisplayName = "Stalking"),
	Prowling = 1 UMETA(DisplayName = "Prowling"),
	Harassing = 2 UMETA(DisplayName = "Harassing"),
	Fleeing = 3 UMETA(DisplayName = "Fleeing"),
	Retrieving = 4 UMETA(DisplayName = "Retrieving")
};

class AWorldItem;
class UItemData;

UCLASS()
class UE_DUNGEONCOMPANY_API ACatBurglar : public AAIEntity
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	USceneComponent* DropTransform;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Balancing")
	float ItemDetectionRadius = 500.f;

	UPROPERTY(EditDefaultsOnly)
	TMap<ECatBurglarBehaviorState, UBehaviorTree*> BehaviorTreesForStates;

	UPROPERTY(EditDefaultsOnly, Category = "Balancing")
	float StartFleeingHPUpper = 30.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.f, ClampMax = 1.f), Category = "Balancing")
	float AttackDropsItemOdds = 0.2f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.f), Category = "Balancing")
	float RetrievingRange = 3000.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.f), Category = "Balancing")
	float OutOfRetrievingRangeDespawnTime = 10.f;

	//Movement
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.f), Category = "Balancing|Movement")
	float DefaultSpeed = 350.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.f), Category = "Balancing|Movement")
	float FleeingSpeed = 350.f * 1.6f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.f), Category = "Balancing|Movement")
	float RetrievingSpeed = 350.f * 1.3f;
	
private:
	ECatBurglarBehaviorState IdleBehaviorState;
	ECatBurglarBehaviorState CurrentBehaviorState;

	TArray<AWorldItem*> WorldItemsInSight;
	UItemData* StolenItem;

public:
	inline const ECatBurglarBehaviorState& GetCurrentBehaviorState() const { return CurrentBehaviorState; }

public:
	void SetIdleBehaviorState(ECatBurglarBehaviorState NewIdleState);
	void ResetToIdleBehavior();

public:
	ACatBurglar();

private:
	bool bInRetrievingRange;
	FTimerHandle RetrieveHandle;

protected:
	virtual void Tick(float DeltaSeconds) override;	
	void Retrieve();

public:
	void StealItem(AWorldItem* StealingItem);
	void StartFleeing();

protected:
	void UpdateBehavior(ECatBurglarBehaviorState NewBehaviorState);

	void SetInFleeingRange(bool InInFleeingRange) const;

private:
	bool bHealthBelowFleeingUpper = false;

protected:
	virtual void OnTookDamage() override;

protected:
	virtual void OnPlayerAttackHit(APlayerCharacter* PlayerCharacter) override;

	virtual void HandleSightSense(AActor* Actor, FAIStimulus const Stimulus,
	                              UBlackboardComponent* BlackboardComponent) override;
	virtual void HandleHearingSense(AActor* Actor, FAIStimulus const Stimulus,
	                                UBlackboardComponent* BlackboardComponent) override;
};
