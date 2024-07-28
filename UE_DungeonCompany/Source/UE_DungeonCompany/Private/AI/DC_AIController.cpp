// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DC_AIController.h"
#include "Entities/AIEntity.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "DC_Statics.h"

ADC_AIController::ADC_AIController(FObjectInitializer const& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent")));
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ADC_AIController::OnTargetPerceptionUpdated);

}

void ADC_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AIEntity = Cast<AAIEntity>(InPawn);

	if(!AIEntity)
		return;

	for (UAISenseConfig* config : AIEntity->GetSenseConfigs())
		GetAIPerceptionComponent()->ConfigureSense(*config);

	GetAIPerceptionComponent()->SetDominantSense(AIEntity->GetDominantSense());

	GetAIPerceptionComponent()->RequestStimuliListenerUpdate();

	UBehaviorTree* tree = AIEntity->GetBehaviorTree();

	if(!tree)
		return;

	UBlackboardComponent* b;
	UseBlackboard(tree->BlackboardAsset, b);
	bUsingBlackboard = true;
	Blackboard = b;
	RunBehaviorTree(tree);
}

void ADC_AIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus const Stimulus)
{
	if (!bUsingBlackboard || !IsValid(AIEntity))
		return;

	AIEntity->HandleSenseUpdate(Actor, Stimulus, GetBlackboardComponent());
}

void ADC_AIController::PutAggroOnPlayer(APlayerCharacter* AggroPullingPlayer)
{
	if(!AggroPullingPlayer)
		return;

	GetBlackboardComponent()->SetValueAsObject(FName("TargetPlayer"), AggroPullingPlayer);
	OnTargetingPlayer.Broadcast(AggroPullingPlayer);
}
