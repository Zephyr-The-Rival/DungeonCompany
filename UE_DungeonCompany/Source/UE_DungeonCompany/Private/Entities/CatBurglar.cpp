// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/CatBurglar.h"

#include "EngineUtils.h"
#include "AI/DC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DCGame/DC_GM.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/ItemData.h"
#include "Items/WorldItem.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "WorldActors/LostItemsCollection.h"

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

void ACatBurglar::BeginPlay()
{
	Super::BeginPlay();

	DynMaterialInstance = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	GetMesh()->SetMaterial(0, DynMaterialInstance);
}

void ACatBurglar::ToggleEyesGlow(bool bInEyesGlow) const
{
	DynMaterialInstance->SetScalarParameterValue(FName("Emissive brightness"), bInEyesGlow);
}

void ACatBurglar::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	bool bCurrentlyInRetrievingRange = RetrievingRange < GetDistanceTo(GetClosestPlayer());

	if (bCurrentlyInRetrievingRange && !bInRetrievingRange)
	{
		GetWorld()->GetTimerManager().SetTimer(RetrieveHandle, this, &ACatBurglar::Retrieve,
		                                       OutOfRetrievingRangeDespawnTime);
		bInRetrievingRange = true;
	}
	else if (!bCurrentlyInRetrievingRange && bInRetrievingRange)
	{
		GetWorld()->GetTimerManager().ClearTimer(RetrieveHandle);
		bInRetrievingRange = false;
	}
}

void ACatBurglar::Retrieve()
{
	if (!StolenItem)
	{
		Destroy();
		return;
	}

	TActorIterator<ALostItemsCollection> LostItemIt(GetWorld());

	if (LostItemIt)
		(*LostItemIt)->AddLostItem(StolenItem);

	Destroy();
}

void ACatBurglar::StealItem(AWorldItem* StealingItem)
{
	StolenItem = StealingItem->GetMyData();
	StealingItem->Destroy();

	SetIsStealingItem(true);

	UpdateBehavior(ECatBurglarBehaviorState::Retrieving);
}

void ACatBurglar::StartFleeing()
{
	UpdateBehavior(ECatBurglarBehaviorState::Fleeing);
}

void ACatBurglar::UpdateBehavior(ECatBurglarBehaviorState NewBehaviorState)
{
	if (!BehaviorTreesForStates.Contains(NewBehaviorState))
		return;

	CurrentBehaviorState = NewBehaviorState;
	SetInFleeingRange(NewBehaviorState == ECatBurglarBehaviorState::Fleeing);
	SetActorTickEnabled(
		NewBehaviorState == ECatBurglarBehaviorState::Retrieving || NewBehaviorState ==
		ECatBurglarBehaviorState::Fleeing);

	ToggleEyesGlow(false);

	switch (NewBehaviorState)
	{
	case ECatBurglarBehaviorState::Fleeing:
		GetCharacterMovement()->MaxWalkSpeed = FleeingSpeed;
		break;

	case ECatBurglarBehaviorState::Retrieving:
		GetCharacterMovement()->MaxWalkSpeed = RetrievingSpeed;
		break;

	default:
		GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed;
	}

	RunBehaviorTree(BehaviorTreesForStates[NewBehaviorState]);
}

void ACatBurglar::SetInFleeingRange(bool InInFleeingRange) const
{
	ADC_AIController* aiController = GetController<ADC_AIController>();

	if (aiController)
		aiController->GetBlackboardComponent()->SetValueAsBool("InFleeingRange", InInFleeingRange);
}

void ACatBurglar::OnTookDamage_Implementation()
{
	Super::OnTookDamage_Implementation();

	if (!bHealthBelowFleeingUpper && HP < StartFleeingHPUpper && CurrentBehaviorState <
		ECatBurglarBehaviorState::Fleeing)
	{
		bHealthBelowFleeingUpper = true;
		UpdateBehavior(ECatBurglarBehaviorState::Fleeing);
	}

	if (!StolenItem)
		return;

	FTimerHandle throwUpHandle;

	GetWorld()->GetTimerManager().SetTimer(throwUpHandle, this, &ACatBurglar::ThrowUpItem, ThrowUpDelay);
}

void ACatBurglar::ThrowUpItem()
{
	if (!IsValid(StolenItem))
		return;

	ADC_GM* gameMode = GetWorld()->GetAuthGameMode<ADC_GM>();
	if (!gameMode)
		return;

	gameMode->SpawnWorldItem(StolenItem->MyWorldItemClass, GetDropTransform(),
							 StolenItem->SerializeMyData());
	StolenItem = nullptr;
	SetIsStealingItem(false);
}

void ACatBurglar::OnPlayerAttackHit(APlayerCharacter* PlayerCharacter)
{
	Super::OnPlayerAttackHit(PlayerCharacter);

	if (FMath::FRand() < AttackDropsItemOdds)
		PlayerCharacter->DropRandomItem();

	UpdateBehavior(ECatBurglarBehaviorState::Fleeing);
}

void ACatBurglar::HandleSightSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent)
{
	AWorldItem* worldItem = Cast<AWorldItem>(Actor);
	if (!worldItem)
		return;
	
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

void ACatBurglar::SetIsStealingItem(bool InIsStealing)
{
	if (InIsStealing == IsStealingItem())
		return;

	ToggleAnimationBitFlag(AAIEntity::FLAG_Custom_0);
}

void ACatBurglar::SetAreEyesGlowing(bool InAreEyesGlowing)
{
	if (InAreEyesGlowing == AreEyesGlowing())
		return;

	ToggleAnimationBitFlag(AAIEntity::FLAG_Custom_1);
}
