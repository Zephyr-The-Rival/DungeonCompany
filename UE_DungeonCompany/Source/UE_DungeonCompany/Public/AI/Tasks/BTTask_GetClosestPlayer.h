// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetClosestPlayer.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UBTTask_GetClosestPlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere)
	float MaxDistance = 2000.f;

	UPROPERTY(EditAnywhere)
	bool bResetValueIfNoPlayerFound = true;

public:
	UBTTask_GetClosestPlayer();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
