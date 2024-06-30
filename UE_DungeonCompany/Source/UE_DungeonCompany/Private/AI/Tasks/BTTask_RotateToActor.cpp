// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_RotateToActor.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_RotateToActor::UBTTask_RotateToActor()
{
	NodeName = "RotateToActor";
}

EBTNodeResult::Type UBTTask_RotateToActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* aiController = OwnerComp.GetAIOwner();
	if(!aiController)
		return EBTNodeResult::Failed;

	ACharacter* aiCharacter = aiController->GetPawn<ACharacter>();
	if(!aiCharacter)
		return EBTNodeResult::Failed;

	AActor* targetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
	if(!targetActor)
		return EBTNodeResult::Failed;

	FRotator newRotation = UKismetMathLibrary::FindLookAtRotation(aiCharacter->GetActorLocation(), targetActor->GetActorLocation() + FVector::UpVector * 100);

	newRotation = UKismetMathLibrary::RInterpTo(aiCharacter->GetActorRotation(), newRotation, GetWorld()->DeltaTimeSeconds, RotationSpeed);

	aiCharacter->SetActorRotation(newRotation, ETeleportType::TeleportPhysics);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
