// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_TargetPlayer.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UEnvQueryContext_TargetPlayer : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
	AActor* GetClosestPlayer(APawn* QueryOwner) const;
};
