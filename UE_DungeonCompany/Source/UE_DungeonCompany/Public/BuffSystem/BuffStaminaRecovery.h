// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuffSystem/BuffDebuffBase.h"
#include "BuffStaminaRecovery.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UBuffStaminaRecovery : public UBuffDebuffBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.01f))
	float StaminaGainDelayOverride = 0.01f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1.f))
	float StaminaGainPerSecondMultiplier = 1.f;

private:
	UPROPERTY(Transient)
	float OriginalStaminaGain;

	UPROPERTY(Transient)
	float OriginalStaminaGainPerSecond;

protected:
	virtual void LocalApply() override;
	virtual void LocalRemove() override;
};
