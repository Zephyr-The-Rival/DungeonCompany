// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DC_PC.generated.h"

/**
 * 
 */
 class UVOIPTalker;

UCLASS()
class UE_DUNGEONCOMPANY_API ADC_PC : public APlayerController
{
	GENERATED_BODY()

public:
	ADC_PC();

protected:
	virtual void BeginPlay() override;

};
