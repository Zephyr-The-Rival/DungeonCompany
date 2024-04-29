// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/AI/DC_AIController.h"
#include "Entities/DC_Entity.h"

#include "BehaviorTree\BehaviorTree.h"

ADC_AIController::ADC_AIController(FObjectInitializer const& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void ADC_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ADC_Entity* entity = Cast<ADC_Entity>(InPawn);

	if(!entity)
		return;

	UBehaviorTree* tree = entity->GetBehaviorTree();

	if(!tree)
		return;

	UBlackboardComponent* b;
	UseBlackboard(tree->BlackboardAsset, b);
	Blackboard = b;
	RunBehaviorTree(tree);
}
