// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_StartMovement.h"
#include "AI/DC_AIController.h"
#include "Entities/AIEntity.h"
#include "Entities/QuasoSnake.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_StartMovement::UBTTask_StartMovement()
{
	NodeName = "StartMovement";
}

EBTNodeResult::Type UBTTask_StartMovement::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("starting movement"));

	AAIController* aiController = OwnerComp.GetAIOwner();
	if (!aiController)
		return EBTNodeResult::Failed;

	AAIEntity* aiEntity = aiController->GetPawn<AAIEntity>();
	if (!aiEntity)
		return EBTNodeResult::Failed;

	OwnerComp.GetBlackboardComponent()->SetValueAsBool("MovementStarted", true);
	aiEntity->GetCharacterMovement()->SetMovementMode(DefaultMovementMode);

	if (AQuasoSnake* quaso = Cast<AQuasoSnake>(aiEntity))
	{
		quaso->StopLurking();
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
