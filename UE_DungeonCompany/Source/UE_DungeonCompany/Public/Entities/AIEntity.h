// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/DC_Entity.h"
#include "AIEntity.generated.h"

/**
 * 
 */

class UBehaviorTree;
class APlayerCharacter;

UCLASS()
class UE_DUNGEONCOMPANY_API AAIEntity : public ADC_Entity
{
	GENERATED_BODY()

public:
	AAIEntity();

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;

public:
	inline UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	virtual void AttackPlayer(APlayerCharacter* TargetPlayer);

	bool IsVisibleToPlayers() const;

	virtual void OnDeath_Implementation() override;
	
};
