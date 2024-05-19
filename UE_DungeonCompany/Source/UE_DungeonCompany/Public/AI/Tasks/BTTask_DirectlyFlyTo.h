// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_DirectlyFlyTo.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UBTTask_DirectlyFlyTo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere)
	float AcceptableRadius = 5.f;

	FVector startDirection = FVector::ZeroVector;

public:
	explicit UBTTask_DirectlyFlyTo(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
