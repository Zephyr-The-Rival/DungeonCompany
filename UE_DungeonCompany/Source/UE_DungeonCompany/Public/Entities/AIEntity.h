// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/DC_Entity.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig.h"
#include "AIEntity.generated.h"

/**
 * 
 */

class UBehaviorTree;
class APlayerCharacter;
class UBlackboardComponent;

UCLASS()
class UE_DUNGEONCOMPANY_API AAIEntity : public ADC_Entity
{
	GENERATED_BODY()

public:
	AAIEntity();

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;

protected:
	UPROPERTY(EditAnywhere, Instanced, Category = "AI|Perception")
	TArray<UAISenseConfig*> SenseConfigs;

	UPROPERTY(EditAnywhere, Category = "AI|Perception")
	TSubclassOf<UAISense> DominantSense;

public:
	inline UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
	inline const TArray<UAISenseConfig*>& GetSenseConfigs() const { return SenseConfigs; }
	inline TSubclassOf<UAISense> GetDominantSense() const { return DominantSense; }

	virtual void AttackPlayer(APlayerCharacter* TargetPlayer);

	bool IsVisibleToPlayers() const;

protected:
	UAISenseConfig* GetSenseConfig(FAISenseID SenseID);

public:
	virtual void HandleSenseUpdate(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent);

protected:
	virtual void HandleSightSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent);
	virtual void HandleHearingSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent);

public:
	virtual void OnDeath_Implementation() override;
	
};
