// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/CatBurglar.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "DCGame/DC_GM.h"
#include "Items/ItemData.h"
#include "Items/WorldItem.h"
#include "PlayerCharacter/PlayerCharacter.h"

void ACatBurglar::SetIdleBehaviorState(ECatBurglarBehaviorState NewIdleState)
{
	IdleBehaviorState = NewIdleState;

	if (CurrentBehaviorState > ECatBurglarBehaviorState::Harassing)
		return;

	UpdateBehavior(IdleBehaviorState);
}

void ACatBurglar::ResetToIdleBehavior()
{
	UpdateBehavior(IdleBehaviorState);
}

ACatBurglar::ACatBurglar()
	: IdleBehaviorState(ECatBurglarBehaviorState::Stalking), CurrentBehaviorState(ECatBurglarBehaviorState::Stalking),
	  StolenItem(nullptr)
{
	BehaviorTreesForStates.Add((ECatBurglarBehaviorState)0);
	BehaviorTreesForStates.Add((ECatBurglarBehaviorState)1);
	BehaviorTreesForStates.Add((ECatBurglarBehaviorState)2);
	BehaviorTreesForStates.Add((ECatBurglarBehaviorState)3);
	BehaviorTreesForStates.Add((ECatBurglarBehaviorState)4);

	DropTransform = CreateDefaultSubobject<USceneComponent>(TEXT("DropTransform"));
	DropTransform->SetupAttachment(RootComponent);
	DropTransform->SetRelativeLocation(FVector(150, 0, 20));
}

void ACatBurglar::StealItem(AWorldItem* StealingItem)
{
	StolenItem = StealingItem->GetMyData();
	StealingItem->Destroy();

	UpdateBehavior(ECatBurglarBehaviorState::Retrieving);
}

void ACatBurglar::UpdateBehavior(ECatBurglarBehaviorState NewBehaviorState)
{
	CurrentBehaviorState = NewBehaviorState;

	if (!BehaviorTreesForStates.Contains(NewBehaviorState))
		return;

	RunBehaviorTree(BehaviorTreesForStates[NewBehaviorState]);
}

void ACatBurglar::OnTookDamage()
{
	Super::OnTookDamage();

	if (!bHealthBelowFleeingUpper && HP < StartFleeingHPUpper && CurrentBehaviorState !=
		ECatBurglarBehaviorState::Fleeing)
	{
		bHealthBelowFleeingUpper = true;
		UpdateBehavior(ECatBurglarBehaviorState::Fleeing);
	}

	if (!StolenItem)
		return;
	
	if (ADC_GM* gameMode = GetWorld()->GetAuthGameMode<ADC_GM>())
		gameMode->SpawnWorldItem(StolenItem->MyWorldItemClass, DropTransform->GetComponentTransform(),
		                         StolenItem->SerializeMyData());
}

void ACatBurglar::OnPlayerAttackHit(APlayerCharacter* PlayerCharacter)
{
	Super::OnPlayerAttackHit(PlayerCharacter);

	TakeDamage(100000);

	PlayerCharacter->DropRandomItem();

	UpdateBehavior(ECatBurglarBehaviorState::Fleeing);
}

void ACatBurglar::HandleSightSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent)
{
	AWorldItem* worldItem = Cast<AWorldItem>(Actor);
	if (!worldItem)
	{
		Super::HandleSightSense(Actor, Stimulus, BlackboardComponent);
		return;
	}

	if (!Stimulus.WasSuccessfullySensed())
	{
		WorldItemsInSight.Remove(worldItem);

		if (BlackboardComponent->GetValueAsObject("TargetItem") != worldItem)
			return;

		BlackboardComponent->ClearValue("TargetItem");

		if (WorldItemsInSight.Num() > 0)
			BlackboardComponent->SetValueAsObject("TargetItem", WorldItemsInSight[0]);

		return;
	}

	if (!worldItem->WasDroppedByPlayer())
		return;

	WorldItemsInSight.Add(worldItem);

	if (IsValid(BlackboardComponent->GetValueAsObject("TargetItem")))
		return;

	BlackboardComponent->SetValueAsObject("TargetItem", worldItem);
}

void ACatBurglar::HandleHearingSense(AActor* Actor, FAIStimulus const Stimulus,
                                     UBlackboardComponent* BlackboardComponent)
{
	Super::HandleHearingSense(Actor, Stimulus, BlackboardComponent);
}
