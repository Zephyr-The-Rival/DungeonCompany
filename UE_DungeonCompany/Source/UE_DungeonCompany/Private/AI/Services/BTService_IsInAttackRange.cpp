// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_IsInAttackRange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTService_IsInAttackRange::UBTService_IsInAttackRange()
{
	bNotifyBecomeRelevant = true;
	NodeName = "IsInAttackRange";
}

#define WriteResult(result) { OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), result); return;}

void UBTService_IsInAttackRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	AActor* targetPlayer = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetPlayer"));

	if (!targetPlayer)
		WriteResult(false);

	AAIController* aiController = OwnerComp.GetAIOwner();

	if (!aiController)
		WriteResult(false);

	APawn* aiPawn = aiController->GetPawn();
	if (!aiPawn)
		WriteResult(false);

	FVector targetLocation = targetPlayer->GetActorLocation();

	if(bIgnoreZValue)
		targetLocation.Z = aiPawn->GetActorLocation().Z;

	WriteResult((targetLocation - aiPawn->GetActorLocation()).Length() <= AttackRange);

}
