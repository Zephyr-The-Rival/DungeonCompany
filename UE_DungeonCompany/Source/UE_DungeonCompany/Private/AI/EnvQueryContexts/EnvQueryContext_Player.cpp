// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnvQueryContexts/EnvQueryContext_Player.h"
#include "AI/DC_AIController.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "BehaviorTree/BlackboardComponent.h"

UEnvQueryContext_Player::UEnvQueryContext_Player(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UEnvQueryContext_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	APawn* queryOwner = Cast<APawn>(QueryInstance.Owner.Get());

	if(!queryOwner)
		return;

	ADC_AIController* aiController = Cast<ADC_AIController>(queryOwner->GetController());

	UBlackboardComponent* blackboard = aiController->GetBlackboardComponent();

	if(!blackboard)
		return;

	AActor* targetPlayer = Cast<AActor>(blackboard->GetValueAsObject("TargetPlayer"));

	if(!targetPlayer)
		return;

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, targetPlayer);

}
