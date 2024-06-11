// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuffSystem/BuffDebuffBase.h"
#include "DebuffPoisonGas.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UDebuffPoisonGas : public UBuffDebuffBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Balancing|Cough");
	float CoughInterval = 2.f;

	FTimerHandle CoughHandle;

	UPROPERTY(EditAnywhere, Category = "Balancing|Damage")
	float DamageIntervalSeconds = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Damage")
	float DamagePerSecond = 2.f;

	FTimerHandle DamageHandle;

protected:
	virtual void Apply() override;
	virtual void AuthorityApply() override;

	virtual void Remove() override;
	virtual void AuthorityRemove() override;
	
};
