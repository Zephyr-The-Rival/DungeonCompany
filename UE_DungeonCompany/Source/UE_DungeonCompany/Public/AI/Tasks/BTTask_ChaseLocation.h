// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ChaseLocation.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UBTTask_ChaseLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.f))
	float MinMoveDistance = 0.f;

public:
	explicit UBTTask_ChaseLocation(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
