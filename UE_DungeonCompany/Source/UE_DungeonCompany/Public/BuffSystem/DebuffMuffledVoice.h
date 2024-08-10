// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuffSystem/BuffDebuffBase.h"
#include "DebuffMuffledVoice.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UE_DUNGEONCOMPANY_API UDebuffMuffledVoice : public UBuffDebuffBase
{
	GENERATED_BODY()

private:
	USoundEffectSourcePresetChain* MuffledSoundEffect;

public:
	UDebuffMuffledVoice();

protected:
	virtual void Apply() override;

	virtual void Remove() override;
	
};
