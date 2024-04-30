// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ClearBlackboardValue.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearBlackboardValue::UBTTask_ClearBlackboardValue(FObjectInitializer const& ObjectInitializer)
	:Super(ObjectInitializer)
{
	NodeName = "Clear Blackboard Value";
}

EBTNodeResult::Type UBTTask_ClearBlackboardValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
