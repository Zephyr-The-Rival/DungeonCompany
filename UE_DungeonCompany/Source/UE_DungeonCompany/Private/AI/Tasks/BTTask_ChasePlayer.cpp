// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ChasePlayer.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "AI/DC_AIController.h"
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

	APlayerCharacter* targetPlayer = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));

	if(!targetPlayer)
		return EBTNodeResult::Failed;

	FVector targetLocation = targetPlayer->GetActorLocation();

	OwnerComp.GetBlackboardComponent()->SetValueAsVector("TargetLocation", targetLocation);

	if (targetPlayer->IsDead())
	{
		OwnerComp.GetBlackboardComponent()->ClearValue("TargetPlayer");

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	UAIBlueprintHelperLibrary::SimpleMoveToActor(aiController, targetPlayer);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}