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
#include "Net/UnrealNetwork.h"

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
	if (TargetPlayer->IsDead())
	{
		SetTargetPlayer(nullptr);
		return;
	}

	FVector attackDirection = TargetPlayer->GetActorLocation() - GetActorLocation();
	attackDirection.Normalize();

	FTimerDelegate delegate = FTimerDelegate::CreateUObject(this, &AAIEntity::ExecuteAttack, attackDirection);
	GetWorld()->GetTimerManager().SetTimer(ExecuteAttackHandle, delegate, AttackDelay, false);

	SetInAttackOnBlackboard(true);

	SetActorRotation(attackDirection.Rotation());
}

void AAIEntity::ExecuteAttack(FVector Direction)
{
	FCollisionShape shape = FCollisionShape::MakeSphere(AttackRadius);

	TArray<FHitResult> hits;

	FVector start = GetActorLocation();
	FVector end = start + Direction * AttackRange;

	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	GetWorld()->SweepMultiByChannel(hits, start, end, FQuat(), ECC_Pawn, shape, params);

	int hitsNum = hits.Num();

	for (int i = 0; i < hitsNum; ++i)
	{
		APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(hits[i].GetActor());

		if(!playerCharacter)
			continue;

		playerCharacter->TakeDamage(AttackDamage);
	}

	SetInAttackOnBlackboard(false);
}

void AAIEntity::SetInAttackOnBlackboard(bool InAttack)
{
	ADC_AIController* aiController = GetController<ADC_AIController>();

	if (aiController)
		aiController->GetBlackboardComponent()->SetValueAsBool("AttackingPlayer", InAttack);
}

void AAIEntity::SetTargetPlayer(APlayerCharacter* TargetPlayer)
{
	ADC_AIController* aiController = GetController<ADC_AIController>();

	if (aiController)
		aiController->GetBlackboardComponent()->SetValueAsObject("TargetPlayer", TargetPlayer);

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

void AAIEntity::SetIsAttacking(bool InAttacking)
{
	if(InAttacking == IsAttacking())
		return;

	ToggleAnimationBitFlag(FLAG_Attacking);
}

void AAIEntity::SetAnimationBitFlag(EAnimationFlags InBit)
{
	AnimationFlags |= InBit;
}

void AAIEntity::ClearAnimatinoBitFlag(EAnimationFlags InBit)
{
	AnimationFlags &= ~InBit;
}

void AAIEntity::ToggleAnimationBitFlag(EAnimationFlags InBit)
{
	AnimationFlags ^= InBit;
}

void AAIEntity::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAIEntity, AnimationFlags);
}
