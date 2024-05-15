// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_ChangeSpeed.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTService_ChangeSpeed::UBTService_ChangeSpeed()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Change Speed";
}

void UBTService_ChangeSpeed::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	AAIController* aiController = OwnerComp.GetAIOwner();

	if(!aiController)
		return;

	ACharacter* character = Cast<ACharacter>(aiController->GetPawn());
	if(!character)
		return;

	character->GetCharacterMovement()->MaxWalkSpeed = Speed;
}
