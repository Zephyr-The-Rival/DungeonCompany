// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_FindPlayerLocation.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTTask_FindPlayerLocation::UBTTTask_FindPlayerLocation(FObjectInitializer const& ObjectInitializer)
	:Super(ObjectInitializer)
{
	NodeName = "Find Player Location";
}

EBTNodeResult::Type UBTTTask_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACharacter* character = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetPlayer"));

	if(!character)
		return EBTNodeResult::Failed;

	FVector charLocation = character->GetActorLocation();

	if (!SearchRandom)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), charLocation);
		return EBTNodeResult::Succeeded;
	}

	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());

	if(!navSys)
		return EBTNodeResult::Failed;

	FNavLocation navLocation;

	if (navSys->GetRandomPointInNavigableRadius(charLocation, SearchRadius, navLocation))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), charLocation);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
