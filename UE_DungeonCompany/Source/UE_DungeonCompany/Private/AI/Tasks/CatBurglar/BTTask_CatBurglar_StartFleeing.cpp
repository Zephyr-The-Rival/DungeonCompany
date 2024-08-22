// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/CatBurglar/BTTask_CatBurglar_StartFleeing.h"

#include "AI/DC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Entities/CatBurglar.h"

UBTTask_CatBurglar_StartFleeing::UBTTask_CatBurglar_StartFleeing()
{
	NodeName = "SetBehaviorState";
}

EBTNodeResult::Type UBTTask_CatBurglar_StartFleeing::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
                                                                     uint8* NodeMemory)
{
	ADC_AIController* aiController = Cast<ADC_AIController>(OwnerComp.GetAIOwner());
	if (!aiController)
		return EBTNodeResult::Failed;

	ACatBurglar* catBurglar = aiController->GetPawn<ACatBurglar>();

	if(!catBurglar)
		return EBTNodeResult::Failed;

	catBurglar->StartFleeing();
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
