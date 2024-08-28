// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_IsInRange.h"

#include "AIController.h"
#include "EngineUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

UBTService_IsInRange::UBTService_IsInRange()
{
	bNotifyBecomeRelevant = true;
	bNotifyTick = true;
	NodeName = "IsInRange";
}

void UBTService_IsInRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(WriteResultTo.SelectedKeyName, IsInRange(OwnerComp));
}

void UBTService_IsInRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(WriteResultTo.SelectedKeyName, IsInRange(OwnerComp));
}

bool UBTService_IsInRange::IsInRange(UBehaviorTreeComponent& OwnerComp) const
{
	AAIController* aiController = OwnerComp.GetAIOwner();

	if (!aiController)
		return false;

	APawn* aiPawn = aiController->GetPawn();
	if (!aiPawn)
		return false;

	if (bCheckAgainstActorClass)
		return IsInRangeOfActorType(aiPawn);

	AActor* checkingActor = Cast<AActor>(
		OwnerComp.GetBlackboardComponent()->GetValueAsObject(CheckingActor.SelectedKeyName));

	FVector targetLocation = checkingActor
		                         ? checkingActor->GetActorLocation()
		                         : OwnerComp.GetBlackboardComponent()->GetValueAsVector(CheckingActor.SelectedKeyName);

	if (bIgnoreZValue)
		targetLocation.Z = aiPawn->GetActorLocation().Z;

	return (targetLocation - aiPawn->GetActorLocation()).Length() <= Range;
}

bool UBTService_IsInRange::IsInRangeOfActorType(const AActor* InActor) const
{
	for (TActorIterator<AActor> It(GetWorld(), ActorClass); It; ++It)
	{
		if ((*It)->GetDistanceTo(InActor) < Range)
			return true;
	}

	return false;
}
