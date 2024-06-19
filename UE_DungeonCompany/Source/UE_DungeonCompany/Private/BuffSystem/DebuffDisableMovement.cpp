// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffSystem/DebuffDisableMovement.h"

#include "Entities/DC_Entity.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDebuffDisableMovement::Apply()
{
	Super::Apply();

	OriginalMovementMode = GetOuterEntity()->GetCharacterMovement()->MovementMode;

	GetOuterEntity()->GetCharacterMovement()->DisableMovement();
}

void UDebuffDisableMovement::Remove()
{
	Super::Remove();

	GetOuterEntity()->GetCharacterMovement()->SetMovementMode(OriginalMovementMode);
}