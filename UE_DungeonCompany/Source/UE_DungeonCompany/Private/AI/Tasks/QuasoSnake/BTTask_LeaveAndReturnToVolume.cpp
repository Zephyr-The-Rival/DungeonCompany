// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/QuasoSnake/BTTask_LeaveAndReturnToVolume.h"
#include "Entities/QuasoSnake.h"

#include "AI/DC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_LeaveAndReturnToVolume::UBTTask_LeaveAndReturnToVolume(FObjectInitializer const& ObjectInitializer)
	:Super(ObjectInitializer)
{
	NodeName = TEXT("LeaveAndReturnToVolume");
}

EBTNodeResult::Type UBTTask_LeaveAndReturnToVolume::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADC_AIController* aiController = Cast<ADC_AIController>(OwnerComp.GetAIOwner());
	if (!aiController)
		return EBTNodeResult::Failed;

	AQuasoSnake* quasoPawn = aiController->GetPawn<AQuasoSnake>();

	if(!quasoPawn)
		return EBTNodeResult::Failed;

	quasoPawn->ReturnToVolume();

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
