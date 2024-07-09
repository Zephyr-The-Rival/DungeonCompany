// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_AttackPlayer.h"
#include "AI/DC_AIController.h"
#include "Entities/AIEntity.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AttackPlayer::UBTTask_AttackPlayer()
{
	NodeName = "AttackPlayer";
}

EBTNodeResult::Type UBTTask_AttackPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* aiController = OwnerComp.GetAIOwner();

	if (!aiController)
		return EBTNodeResult::Failed;

	AAIEntity* aiEntity = Cast<AAIEntity>(aiController->GetPawn());
	if (!aiEntity)
		return EBTNodeResult::Failed;

	APlayerCharacter* targetPlayer = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
	if (!targetPlayer)
		return EBTNodeResult::Failed;

	aiEntity->AttackPlayer(targetPlayer);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
