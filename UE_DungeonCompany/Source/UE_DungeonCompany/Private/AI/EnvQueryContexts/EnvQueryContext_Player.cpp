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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "1");

	if(!queryOwner)
		return;

	ADC_AIController* aiController = Cast<ADC_AIController>(queryOwner->GetController());

	UBlackboardComponent* blackboard = aiController->GetBlackboardComponent();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "2");


	if(!blackboard)
		return;

	AActor* targetPlayer = Cast<AActor>(blackboard->GetValueAsObject("TargetPlayer"));

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "3");


	if(!targetPlayer)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, targetPlayer->GetName());

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "4");


	UEnvQueryItemType_Actor::SetContextHelper(ContextData, targetPlayer);

}
