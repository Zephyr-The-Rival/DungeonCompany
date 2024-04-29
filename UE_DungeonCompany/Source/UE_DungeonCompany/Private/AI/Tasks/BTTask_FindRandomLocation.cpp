// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_FindRandomLocation.h"
#include "AI/DC_AIController.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation(FObjectInitializer const& ObjectInitializer)
	:Super(ObjectInitializer)
{
	NodeName = "Find Random Location In NavMesh";
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADC_AIController* aiController = Cast<ADC_AIController>(OwnerComp.GetAIOwner());
	if(!aiController)
		return EBTNodeResult::Failed;

	APawn* pawn = aiController->GetPawn();
	if(!pawn)
		return EBTNodeResult::Failed;

	FVector origin = pawn->GetActorLocation();

	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if(!navSys)
		return EBTNodeResult::Failed;

	FNavLocation location;

	if(navSys->GetRandomPointInNavigableRadius(origin, SearchRadius, location))
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), location.Location);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;

}
