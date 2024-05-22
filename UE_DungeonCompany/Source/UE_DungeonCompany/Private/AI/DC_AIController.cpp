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

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent")));
	SetupPerceptionSystem();
}

void ADC_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AAIEntity* entity = Cast<AAIEntity>(InPawn);

	if(!entity)
		return;

	UBehaviorTree* tree = entity->GetBehaviorTree();

	if(!tree)
		return;

	UBlackboardComponent* b;
	UseBlackboard(tree->BlackboardAsset, b);
	bUsingBlackboard = true;
	Blackboard = b;
	RunBehaviorTree(tree);
}

void ADC_AIController::SetupPerceptionSystem()
{
	SightConfig->SightRadius = 500.f;
	SightConfig->LoseSightRadius = SightConfig->SightRadius + 300.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 800.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());

	HearingConfig->HearingRange = 3000.f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	GetPerceptionComponent()->ConfigureSense(*HearingConfig);

	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ADC_AIController::OnTargetPerceptionUpdated);
}

void ADC_AIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus const Stimulus)
{
	if (!bUsingBlackboard)
		return;

	if (Stimulus.Type == SightConfig->GetSenseID())
		HandleSightSense(Actor, Stimulus);
	else if(Stimulus.Type == HearingConfig->GetSenseID())
		HandleHearingSense(Stimulus);
}

void ADC_AIController::HandleSightSense(AActor* Actor, FAIStimulus const Stimulus)
{
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(Actor);

	if (!playerCharacter)
		return;

	if (Stimulus.WasSuccessfullySensed())
		GetBlackboardComponent()->SetValueAsObject("TargetPlayer", Actor);
	else
		GetBlackboardComponent()->ClearValue("TargetPlayer");

}

void ADC_AIController::HandleHearingSense(FAIStimulus const Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		GetBlackboardComponent()->SetValueAsVector("TargetLocation", Stimulus.StimulusLocation);
		GetBlackboardComponent()->SetValueAsBool("NewTarget", true);
	}
}