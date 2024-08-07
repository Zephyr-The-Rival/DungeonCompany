// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_CatBurglar_StealItem.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UBTTask_CatBurglar_StealItem : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_CatBurglar_StealItem();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;	
	
};
