// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuffSystem/BuffDebuffBase.h"
#include "DebuffBlockInputs.generated.h"

/**
 * 
 */
class UEnhancedInputLocalPlayerSubsystem;
class UInputMappingContext;

UCLASS(Blueprintable)
class UE_DUNGEONCOMPANY_API UDebuffBlockInputs : public UBuffDebuffBase
{
	GENERATED_BODY()

private:
	UEnhancedInputLocalPlayerSubsystem* BlockingInputSystem;
	UInputMappingContext* BlockingContext;

protected:
	virtual void LocalApply() override;

	virtual void LocalRemove() override;
	
};
