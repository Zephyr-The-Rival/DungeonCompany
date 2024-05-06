// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_FleeFromPlayer.h"
#include "AI/DC_AIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_FleeFromPlayer::UBTTask_FleeFromPlayer(FObjectInitializer const& ObjectInitializer)
	:Super(ObjectInitializer)
{
	NodeName = "Flee From Player";
}

EBTNodeResult::Type UBTTask_FleeFromPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADC_AIController* aiController = Cast<ADC_AIController>(OwnerComp.GetAIOwner());

	AActor* selfActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));
	AActor* fleeFromActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));

	if(!selfActor || !fleeFromActor || !aiController)
		return EBTNodeResult::Failed;

	FVector ownLocation = selfActor->GetActorLocation();

	FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(ownLocation, fleeFromActor->GetActorLocation());

	FVector targetOffset = UKismetMathLibrary::GetForwardVector(lookAtRotation) * -500;
	FVector targetLocation = ownLocation + targetOffset;

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(aiController, targetLocation);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	return EBTNodeResult::Succeeded;
}
