// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffSystem/DebuffBlockInputs.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "EnhancedInputSubsystems.h"

void UDebuffBlockInputs::LocalApply()
{
	APlayerCharacter* character = GetOuterEntity<APlayerCharacter>();

	if (!character)
		return;

	BlockingInputSystem = character->GetInputLocalPlayer();

	if (!BlockingInputSystem)
		return;

	BlockingContext = character->GetCharacterInputMapping();

	BlockingInputSystem->RemoveMappingContext(BlockingContext);
}

void UDebuffBlockInputs::LocalRemove()
{
	if(!IsValid(BlockingInputSystem))
		return;

	BlockingInputSystem->AddMappingContext(BlockingContext, 0);
}
