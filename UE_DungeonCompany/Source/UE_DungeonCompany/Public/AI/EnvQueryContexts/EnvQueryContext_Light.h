// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_Light.generated.h"

/**
 * 
 */
class ALight;

UCLASS()
class UE_DUNGEONCOMPANY_API UEnvQueryContext_Light : public UEnvQueryContext
{
	GENERATED_BODY()
public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
	void GetAllLights(TArray<AActor*>& Out) const;

	
};
