// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/AIEntity.h"
#include "AI/DC_AIController.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "DC_Statics.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"

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

void AAIEntity::BeginPlay()
{
	Super::BeginPlay();

	if(!HasAuthority())
		return;

	ADC_AIController* aiController = GetController<ADC_AIController>();

	if (!aiController)
		return;

	aiController->OnTargetingPlayer.AddDynamic(this, &AAIEntity::OnTargetingPlayer);
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

UAISenseConfig* AAIEntity::GetSenseConfig(FAISenseID SenseID)
{
	AAIController* aiController = GetController<AAIController>();

	if(!aiController)
		return nullptr;

	return aiController->GetAIPerceptionComponent()->GetSenseConfig(SenseID);
}

void AAIEntity::UpdatePerception()
{
	AAIController* aiController = GetController<AAIController>();

	if (!aiController)
		return;

	aiController->GetAIPerceptionComponent()->RequestStimuliListenerUpdate();
}

void AAIEntity::HandleSenseUpdate(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent)
{
	static const FAISenseID sightID = UAISense::GetSenseID<UAISenseConfig_Sight>();
	static const FAISenseID hearingID = UAISense::GetSenseID<UAISenseConfig_Hearing>();

	if (Stimulus.Type == sightID)
		HandleSightSense(Actor, Stimulus, BlackboardComponent);
	else if (Stimulus.Type == hearingID)
		HandleHearingSense(Actor, Stimulus, BlackboardComponent);

}

void AAIEntity::OnTargetingPlayer_Implementation(APlayerCharacter* Target)
{
}

void AAIEntity::HandleSightSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Stimulus.Type.Name.ToString() + FString::FromInt(Stimulus.WasSuccessfullySensed()));

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