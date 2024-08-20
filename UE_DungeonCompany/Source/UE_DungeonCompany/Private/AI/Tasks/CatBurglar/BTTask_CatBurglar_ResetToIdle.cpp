// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/CatBurglar/BTTask_CatBurglar_ResetToIdle.h"
#include "Entities/CatBurglar.h"

#include "AI/DC_AIController.h"

UBTTask_CatBurglar_ResetToIdle::UBTTask_CatBurglar_ResetToIdle()
{
	NodeName = "Reset To Idle Behavior";
}

EBTNodeResult::Type UBTTask_CatBurglar_ResetToIdle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADC_AIController* aiController = Cast<ADC_AIController>(OwnerComp.GetAIOwner());
	if (!aiController)
		return EBTNodeResult::Failed;

	ACatBurglar* catBurglar = aiController->GetPawn<ACatBurglar>();

	if (!catBurglar)
		return EBTNodeResult::Failed;

	catBurglar->ResetToIdleBehavior();

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
