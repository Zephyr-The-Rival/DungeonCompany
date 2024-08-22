// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnvQueryContexts/EnvQueryContext_TargetPlayer.h"
#include "AI/DC_AIController.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "EngineUtils.h"

void UEnvQueryContext_TargetPlayer::ProvideContext(FEnvQueryInstance& QueryInstance,
                                                   FEnvQueryContextData& ContextData) const
{
	APawn* queryOwner = Cast<APawn>(QueryInstance.Owner.Get());

	if (!queryOwner)
		return;

	ADC_AIController* aiController = Cast<ADC_AIController>(queryOwner->GetController());

	if (!aiController)
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, GetClosestPlayer(queryOwner));
		return;
	}

	UBlackboardComponent* blackboard = aiController->GetBlackboardComponent();

	if (!blackboard)
		return;

	AActor* targetPlayer = Cast<AActor>(blackboard->GetValueAsObject("TargetPlayer"));

	if (!targetPlayer)
		return;

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, targetPlayer);
}

AActor* UEnvQueryContext_TargetPlayer::GetClosestPlayer(APawn* QueryOwner) const
{
	APlayerCharacter* closestPlayer = nullptr;
	float closestDistance = -1.f;

	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		APlayerCharacter* currentPlayer = *It;

		if (currentPlayer->IsDead())
			continue;

		float currentDistance = QueryOwner->GetDistanceTo(currentPlayer);

		if (closestDistance < 0.f || currentDistance < closestDistance)
		{
			closestPlayer = currentPlayer;
			closestDistance = currentDistance;
		}
	}
	return closestPlayer;
}
