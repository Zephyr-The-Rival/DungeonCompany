// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/AI/Tasks/BTTask_ChasePlayer.h"

#include "Entities/AI/DC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UBTTask_ChasePlayer::UBTTask_ChasePlayer(FObjectInitializer const& ObjectInitializer)
	:Super(ObjectInitializer)
{
	NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADC_AIController* aiController = Cast<ADC_AIController>(OwnerComp.GetAIOwner());
	if (!aiController)
		return EBTNodeResult::Failed;

	FVector playerLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(aiController, playerLocation);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}