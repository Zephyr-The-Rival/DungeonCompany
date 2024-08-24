// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnvQueryContexts/EnvQueryContext_Player.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "EngineUtils.h"

void UEnvQueryContext_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	TArray<AActor*> playerCharacters;

	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
		playerCharacters.Add(*It);

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, playerCharacters);
}
