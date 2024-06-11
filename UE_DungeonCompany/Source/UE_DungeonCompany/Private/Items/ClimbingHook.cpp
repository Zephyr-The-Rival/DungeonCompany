// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook.h"
#include "PlayerCharacter/PlayerCharacter.h"

AClimbingHook::AClimbingHook()
{
}

void AClimbingHook::Interact(APawn* InteractingPawn)
{

	if(PartnerHook)
	if (InteractingPawn == CharacterHoldingPartnerHook)
	{	
		return;
	}
	
	Super::Interact(InteractingPawn);
}

void AClimbingHook::TriggerPrimaryAction_Implementation(APlayerCharacter* User)
{
	
}
