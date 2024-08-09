// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuffDebuffBase.h"
#include "UObject/Object.h"
#include "DebuffExaustion.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UE_DUNGEONCOMPANY_API UDebuffExaustion : public UBuffDebuffBase
{
	GENERATED_BODY()

protected:

	void LocalApply() override;
	void LocalRemove() override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Balancing")
	float StaminaRecoveryFactor = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing")
	float TimeUntilExaustion = 420;

public:
	float GetStaminaRecoveryFactor() const {return this-> StaminaRecoveryFactor;}

	float GetTimeUntulExaustion() const {return this-> TimeUntilExaustion;}
};
