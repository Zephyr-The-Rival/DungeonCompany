// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_AggroClosestPlayer.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "AI/DC_AIController.h"

#include "EngineUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Entities/AIEntity.h"

UBTTask_AggroClosestPlayer::UBTTask_AggroClosestPlayer()
{
	NodeName = "AggroClosestPlayer";
}

EBTNodeResult::Type UBTTask_AggroClosestPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* aiController = OwnerComp.GetAIOwner();

	if(!aiController)
		return EBTNodeResult::Failed;

	AAIEntity* aiPawn = aiController->GetPawn<AAIEntity>();

	if (!aiPawn)
		return EBTNodeResult::Failed;

	APlayerCharacter* closestPlayer = nullptr;
	float closestDistance = MaxDistance + 1;

	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		APlayerCharacter* currentPlayer = *It;

		float distance = aiPawn->GetDistanceTo(currentPlayer);

		if(currentPlayer->IsDead() || distance > MaxDistance || distance > closestDistance)
			continue;

		closestDistance = distance;	
		closestPlayer = currentPlayer;
	}

	if(closestPlayer || bResetValueIfNoPlayerFound)
		aiPawn->SetTargetPlayer(closestPlayer);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);	
	return EBTNodeResult::Succeeded;
}
