// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DC_AIController.h"
#include "Entities/DC_Entity.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

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

void ADC_AIController::SetupPerceptionSystem()
{
	SightConfig->SightRadius = 500.f;
	SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 520.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ADC_AIController::OnTargetPerceptionUpdated);

	HearingConfig->HearingRange = 3000.f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	GetPerceptionComponent()->ConfigureSense(*HearingConfig);
}

void ADC_AIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus const Stimulus)
{
	HandleSightSense(Actor, Stimulus);
}

void ADC_AIController::HandleSightSense(AActor* Actor, FAIStimulus const Stimulus)
{
	if(Stimulus.Type != SightConfig->GetSenseID())
		return;

	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(Actor);

	if (!playerCharacter)
		return;

	bool successfullySensed = Stimulus.WasSuccessfullySensed();
	GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", successfullySensed);
	if (successfullySensed)
		GetBlackboardComponent()->SetValueAsObject("PlayerChasing", Actor);
}

void ADC_AIController::HandleHearingSense(FAIStimulus const Stimulus)
{
	if (Stimulus.Type != HearingConfig->GetSenseID())
		return;

	GetBlackboardComponent()->SetValueAsVector("TargetLocation", Stimulus.StimulusLocation);
}
