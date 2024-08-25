// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RotateToActor.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UBTTask_RotateToActor : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	float RotationSpeed = 15.f;

	UPROPERTY(EditAnywhere)
	bool bRotateOnlyYaw = false;

public:
	UBTTask_RotateToActor();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
