// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/AIEntity.h"
#include "AI/DC_AIController.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "DC_Statics.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"

AAIEntity::AAIEntity()
{
	AIControllerClass = ADC_AIController::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.f, 0.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	GetMesh()->SetCollisionProfileName("EntityMesh");
	GetMesh()->SetGenerateOverlapEvents(true);

	GetMesh()->SetCollisionProfileName("EntityMesh");
	GetMesh()->SetGenerateOverlapEvents(true);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	UAISenseConfig_Sight* sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	sightConfig->SightRadius = 500.f;
	sightConfig->LoseSightRadius = sightConfig->SightRadius + 300.f;
	sightConfig->PeripheralVisionAngleDegrees = 60.f;
	sightConfig->SetMaxAge(5.f);
	sightConfig->AutoSuccessRangeFromLastSeenLocation = 800.f;
	sightConfig->DetectionByAffiliation.bDetectEnemies = true;
	sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	sightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	sightConfig->NearClippingRadius = 100.f;
	sightConfig->PointOfViewBackwardOffset = 100.f;

	SenseConfigs.Add(sightConfig);

	UAISenseConfig_Hearing* hearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));;

	hearingConfig->HearingRange = 3000.f;
	hearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	hearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	hearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

	SenseConfigs.Add(hearingConfig);

	DominantSense = UAISenseConfig_Sight::StaticClass();

}

void AAIEntity::AttackPlayer(APlayerCharacter* TargetPlayer)
{
	UE_LOG(LogTemp, Log, TEXT("Attacking Player"));
}

bool AAIEntity::IsVisibleToPlayers() const
{
	for (FConstPlayerControllerIterator iterator = GetWorld()->GetPlayerControllerIterator(); iterator; ++iterator)
	{
		if (UDC_Statics::IsActorVisibleToPlayer(iterator->Get(), this))
			return true;

	}

	return false;
}

void AAIEntity::HandleSenseUpdate(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent)
{
	int senseConfigsNum = SenseConfigs.Num();

	TSubclassOf<UAISense> updatingSense;

	for (int i = 0; i < senseConfigsNum; ++i)
	{
		if (Stimulus.Type != SenseConfigs[i]->GetSenseID())
			continue;

		updatingSense = SenseConfigs[i]->StaticClass();
		break;
	}

	if (updatingSense == UAISenseConfig_Sight::StaticClass())
		HandleSightSense(Actor, Stimulus, BlackboardComponent);
	else if (updatingSense == UAISenseConfig_Hearing::StaticClass())
		HandleHearingSense(Actor, Stimulus, BlackboardComponent);
}

void AAIEntity::HandleSightSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent)
{
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(Actor);

	if (!playerCharacter)
		return;

	if (Stimulus.WasSuccessfullySensed() && !playerCharacter->IsDead())
		BlackboardComponent->SetValueAsObject("TargetPlayer", Actor);
	else
		BlackboardComponent->ClearValue("TargetPlayer");
}

void AAIEntity::HandleHearingSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Actor->GetName());

	if (!Stimulus.WasSuccessfullySensed())
		return;

	BlackboardComponent->SetValueAsVector("TargetLocation", Stimulus.StimulusLocation);
	BlackboardComponent->SetValueAsBool("NewTarget", true);
}

void AAIEntity::OnDeath_Implementation()
{
	Super::OnDeath_Implementation();

	Destroy();
}