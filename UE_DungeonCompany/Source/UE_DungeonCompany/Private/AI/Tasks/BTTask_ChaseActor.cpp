// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ChaseActor.h"

#include "AI/DC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

class ADC_AIController;

UBTTask_ChaseActor::UBTTask_ChaseActor(FObjectInitializer const& ObjectInitializer)
	:Super(ObjectInitializer)
{
	NodeName = TEXT("Chase Actor");
}

EBTNodeResult::Type UBTTask_ChaseActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADC_AIController* aiController = Cast<ADC_AIController>(OwnerComp.GetAIOwner());
	if (!aiController)
		return EBTNodeResult::Failed;

	AActor* targetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));

	if(!targetActor)
		return EBTNodeResult::Failed;

	FVector targetLocation = targetActor->GetActorLocation();

	OwnerComp.GetBlackboardComponent()->SetValueAsVector("TargetLocation", targetLocation);
	UAIBlueprintHelperLibrary::SimpleMoveToActor(aiController, targetActor);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
