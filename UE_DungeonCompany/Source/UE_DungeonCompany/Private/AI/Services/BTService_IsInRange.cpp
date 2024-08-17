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

#define WriteResult(result) { OwnerComp.GetBlackboardComponent()->SetValueAsBool(WriteResultTo.SelectedKeyName, result); return;}

void UBTService_IsInRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* aiController = OwnerComp.GetAIOwner();

	if (!aiController)
		WriteResult(false);

	APawn* aiPawn = aiController->GetPawn();
	if (!aiPawn)
		WriteResult(false);

	if (bCheckAgainstActorClass)

		WriteResult(IsInRangeOfActorType(aiPawn));

	AActor* checkingActor = Cast<AActor>(
		OwnerComp.GetBlackboardComponent()->GetValueAsObject(CheckingActor.SelectedKeyName));

	if (!checkingActor)
		WriteResult(false);

	FVector targetLocation = checkingActor->GetActorLocation();

	if (bIgnoreZValue)
		targetLocation.Z = aiPawn->GetActorLocation().Z;

	WriteResult((targetLocation - aiPawn->GetActorLocation()).Length() <= Range);
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
