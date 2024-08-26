// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffSystem/BuffStaminaRecovery.h"

#include "PlayerCharacter/PlayerCharacter.h"

void UBuffStaminaRecovery::LocalApply()
{
	Super::LocalApply();

	APlayerCharacter* playerCharacter = GetOuterEntity<APlayerCharacter>();

	if (!playerCharacter)
		return;

	OriginalStaminaGain = playerCharacter->GetStaminaGainDelay();
	OriginalStaminaGainPerSecond = playerCharacter->GetStaminaGainPerSecond();

	playerCharacter->SetStaminaGainDelay(StaminaGainDelayOverride);
	playerCharacter->SetStaminaGainPerSecond(OriginalStaminaGainPerSecond * StaminaGainPerSecondMultiplier);
}

void UBuffStaminaRecovery::LocalRemove()
{
	Super::LocalRemove();
	
	APlayerCharacter* playerCharacter = GetOuterEntity<APlayerCharacter>();

	if (!playerCharacter)
		return;

	playerCharacter->SetStaminaGainDelay(OriginalStaminaGain);
	playerCharacter->SetStaminaGainPerSecond(OriginalStaminaGainPerSecond);
}
