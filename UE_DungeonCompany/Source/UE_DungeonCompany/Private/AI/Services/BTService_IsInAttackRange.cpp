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

	UE_LOG(LogTemp, Warning, TEXT("0"))

	if (!targetPlayer)
		WriteResult(false);

	UE_LOG(LogTemp, Warning, TEXT("1"))

	AAIController* aiController = OwnerComp.GetAIOwner();

	if (!aiController)
		WriteResult(false);

	UE_LOG(LogTemp, Warning, TEXT("2"))


	APawn* aiPawn = aiController->GetPawn();
	if (!aiPawn)
		WriteResult(false);

	UE_LOG(LogTemp, Warning, TEXT("3"))


	if(aiPawn->GetDistanceTo(targetPlayer) > AttackRange)
		WriteResult(false);

	UE_LOG(LogTemp, Warning, TEXT("4"))


	WriteResult(true);

}
