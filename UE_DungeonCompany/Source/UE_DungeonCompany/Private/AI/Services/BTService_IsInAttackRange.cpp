// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_IsInAttackRange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTService_IsInAttackRange::UBTService_IsInAttackRange()
{
	bNotifyBecomeRelevant = true;
	NodeName = "IsInAttackRange";
}

void UBTService_IsInAttackRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), IsInAttackingRange(OwnerComp));
}

bool UBTService_IsInAttackRange::IsInAttackingRange(UBehaviorTreeComponent& OwnerComp) const
{
	AActor* targetPlayer = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetPlayer"));

	if (!targetPlayer)
		return false;

	AAIController* aiController = OwnerComp.GetAIOwner();

	if (!aiController)
		return false;

	APawn* aiPawn = aiController->GetPawn();
	if (!aiPawn)
		return false;

	FVector targetLocation = targetPlayer->GetActorLocation();

	if (bIgnoreZValue)
		targetLocation.Z = aiPawn->GetActorLocation().Z;

	return (targetLocation - aiPawn->GetActorLocation()).Length() <= AttackRange;
}
