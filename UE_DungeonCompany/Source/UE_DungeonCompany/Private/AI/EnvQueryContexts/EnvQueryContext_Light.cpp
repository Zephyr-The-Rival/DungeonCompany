// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnvQueryContexts/EnvQueryContext_Light.h"

#include "EngineUtils.h"
#include "Engine/Light.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEnvQueryContext_Light::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	TArray<AActor*> allLights;
	GetAllLights(allLights);

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, allLights);

}

void UEnvQueryContext_Light::GetAllLights(TArray<AActor*>& Out) const
{
	for (TActorIterator<ALight> It(GetWorld()); It; ++It)
		Out.Add(*It);
}
