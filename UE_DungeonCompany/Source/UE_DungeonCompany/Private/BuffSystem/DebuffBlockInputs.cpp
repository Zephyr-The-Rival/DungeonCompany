// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffSystem/DebuffBlockInputs.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "EnhancedInputSubsystems.h"

void UDebuffBlockInputs::LocalApply()
{
	APlayerCharacter* character = GetOuterEntity<APlayerCharacter>();

	if (!character)
		return;

	character->DeactivateCharacterInputMappings();
}

void UDebuffBlockInputs::LocalRemove()
{
	APlayerCharacter* character = GetOuterEntity<APlayerCharacter>();

	if (!character)
		return;

	character->ActivateCharacterInputMappings();
}
