// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_StartMovement.h"
#include "AI/DC_AIController.h"
#include "Entities/AIEntity.h"

#include "GameFramework/CharacterMovementComponent.h"

UBTTask_StartMovement::UBTTask_StartMovement()
{
	NodeName = "StartMovement";
}

EBTNodeResult::Type UBTTask_StartMovement::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADC_AIController* aiController = Cast<ADC_AIController>(OwnerComp.GetAIOwner());
	if (!aiController)
		return EBTNodeResult::Failed;

	AAIEntity* aiEntity = aiController->GetPawn<AAIEntity>();
	if (!aiEntity)
		return EBTNodeResult::Failed;

	aiEntity->GetCharacterMovement()->SetMovementMode(DefaultMovementMode);

	return EBTNodeResult::Succeeded;
}
