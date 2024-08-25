// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/CatBurglar/BTTask_CatBurglar_StealItem.h"
#include "AI/DC_AIController.h"
#include "Items/WorldItem.h"
#include "Entities/CatBurglar.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_CatBurglar_StealItem::UBTTask_CatBurglar_StealItem()
{
	NodeName = "Steal Item";
}

EBTNodeResult::Type UBTTask_CatBurglar_StealItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADC_AIController* aiController = Cast<ADC_AIController>(OwnerComp.GetAIOwner());
	if (!aiController)
		return EBTNodeResult::Failed;

	ACatBurglar* catBurglar = aiController->GetPawn<ACatBurglar>();

	if(!catBurglar)
		return EBTNodeResult::Failed;

	AWorldItem* targetItem = Cast<AWorldItem>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));

	if(!targetItem || !targetItem->WasDroppedByPlayer())
		return EBTNodeResult::Failed;

	catBurglar->StealItem(targetItem);
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
