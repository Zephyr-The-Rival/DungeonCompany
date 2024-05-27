// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_GetClosestPlayer.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "AI/DC_AIController.h"

#include "EngineUtils.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetClosestPlayer::UBTTask_GetClosestPlayer()
{
	NodeName = "GetClosestPlayer";
}

EBTNodeResult::Type UBTTask_GetClosestPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* aiController = OwnerComp.GetAIOwner();

	if(!aiController)
		return EBTNodeResult::Failed;

	APawn* aiPawn = aiController->GetPawn();

	if (!aiPawn)
		return EBTNodeResult::Failed;

	APlayerCharacter* closestPlayer = nullptr;
	float closestDistance = MaxDistance + 1;

	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		APlayerCharacter* currentPlayer = *It;

		float distance = aiPawn->GetDistanceTo(currentPlayer);

		if(distance > MaxDistance || distance > closestDistance)
			continue;

		closestDistance = distance;	
		closestPlayer = currentPlayer;
		
	}

	if(!closestPlayer)
		return EBTNodeResult::Failed;

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), closestPlayer);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	
	return EBTNodeResult::Succeeded;
}
