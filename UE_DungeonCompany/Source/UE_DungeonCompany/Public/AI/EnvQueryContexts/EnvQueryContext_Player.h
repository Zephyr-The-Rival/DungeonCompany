// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_Player.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UEnvQueryContext_Player : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	UEnvQueryContext_Player(const FObjectInitializer& ObjectInitializer);

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
	
};
