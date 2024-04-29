// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DC_AIController.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API ADC_AIController : public AAIController
{
	GENERATED_BODY()

public:
	explicit ADC_AIController(FObjectInitializer const& ObjectInitializer);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	
};
