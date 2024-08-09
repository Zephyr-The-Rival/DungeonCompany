// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/CatBurglar.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Items/WorldItem.h"
#include "PlayerCharacter/PlayerCharacter.h"

void ACatBurglar::SetIdleBehaviorState(ECatBurglarBehaviorState NewIdleState)
{
	IdleBehaviorState = NewIdleState;

	if (CurrentBehaviorState > ECatBurglarBehaviorState::Harassing)
		return;

	UpdateBehavior(IdleBehaviorState);
}

ACatBurglar::ACatBurglar()
{
	BehaviorTreesForStates.Add((ECatBurglarBehaviorState)0);
	BehaviorTreesForStates.Add((ECatBurglarBehaviorState)1);
	BehaviorTreesForStates.Add((ECatBurglarBehaviorState)2);
	BehaviorTreesForStates.Add((ECatBurglarBehaviorState)3);
	BehaviorTreesForStates.Add((ECatBurglarBehaviorState)4);
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

	if (HP < StartFleeingHPUpper && CurrentBehaviorState != ECatBurglarBehaviorState::Fleeing)
		UpdateBehavior(ECatBurglarBehaviorState::Fleeing);
}

void ACatBurglar::OnPlayerAttackHit(APlayerCharacter* PlayerCharacter)
{
	Super::OnPlayerAttackHit(PlayerCharacter);

	PlayerCharacter->DropRandomItem();
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
