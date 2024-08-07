// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_QuasoSnake_LeaveAndReturnToVolume.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UBTTask_QuasoSnake_LeaveAndReturnToVolume : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	explicit UBTTask_QuasoSnake_LeaveAndReturnToVolume(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
