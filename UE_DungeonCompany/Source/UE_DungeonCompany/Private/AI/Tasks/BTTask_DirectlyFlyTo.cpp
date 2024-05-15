// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_DirectlyFlyTo.h"

#include "AI/DC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_DirectlyFlyTo::UBTTask_DirectlyFlyTo(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Directly Fly To";
}

EBTNodeResult::Type UBTTask_DirectlyFlyTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADC_AIController* aiController = Cast<ADC_AIController>(OwnerComp.GetAIOwner());
	if (!aiController)
		return EBTNodeResult::Failed;

	ACharacter* character = Cast<ACharacter>(aiController->GetPawn());

	if (!character)
		return EBTNodeResult::Failed;

	FVector destination = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());

	FVector direction = destination - character->GetActorLocation();

	if (startDirection == FVector::ZeroVector)
		startDirection = direction;

	if (direction.Length() < AcceptableRadius || startDirection.Dot(direction) < 0.f)
	{
		character->GetCharacterMovement()->BrakingDecelerationFlying = 5000.f;
		OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	character->GetCharacterMovement()->BrakingDecelerationFlying = 0.f;

	character->AddMovementInput(direction);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	
	return EBTNodeResult::InProgress;
}
