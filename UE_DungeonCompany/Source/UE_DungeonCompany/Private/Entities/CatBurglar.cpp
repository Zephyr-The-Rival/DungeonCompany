// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/CatBurglar.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Items/WorldItem.h"

void ACatBurglar::StealItem(AWorldItem* StealingItem)
{	
	StealingItem->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	StolenItem = StealingItem;	
}

void ACatBurglar::HandleSightSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent)
{
	AWorldItem* worldItem = Cast<AWorldItem>(Actor);
	if(!worldItem)
	{
		Super::HandleSightSense(Actor, Stimulus, BlackboardComponent);
		return;
	}
	
	if(!Stimulus.WasSuccessfullySensed())
	{
		WorldItemsInSight.Remove(worldItem);

		if(BlackboardComponent->GetValueAsObject("TargetItem") != worldItem)
			return;
		
		BlackboardComponent->ClearValue("TargetItem");

		if(WorldItemsInSight.Num() > 0)
			BlackboardComponent->SetValueAsObject("TargetItem", WorldItemsInSight[0]);
		
		return;
	}

	if(!worldItem->WasDroppedByPlayer())
		return;
	
	WorldItemsInSight.Add(worldItem);

	if(IsValid(BlackboardComponent->GetValueAsObject("TargetItem")))
		return;

	BlackboardComponent->SetValueAsObject("TargetItem", worldItem);

}

void ACatBurglar::HandleHearingSense(AActor* Actor, FAIStimulus const Stimulus,
	UBlackboardComponent* BlackboardComponent)
{
	Super::HandleHearingSense(Actor, Stimulus, BlackboardComponent);
}
