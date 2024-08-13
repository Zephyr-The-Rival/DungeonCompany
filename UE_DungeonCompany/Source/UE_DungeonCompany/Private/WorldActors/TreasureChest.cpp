// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/TreasureChest.h"

#include "PlayerCharacter/PlayerCharacter.h"
#include "UI/PlayerHud/PlayerHud.h"

ATreasureChest::ATreasureChest()
{
	this->bInteractOnServer=true;
	this->bNeedsHolding=true;
	this->HoldInteractTime=5;
}

void ATreasureChest::OnHovered(APlayerCharacter* PlayerCharacter)
{
	if(!bUsed)
	{
		IInteractable::OnHovered(PlayerCharacter);
		PlayerCharacter->GetMyHud()->ShowTextInteractPrompt("Hold to Open");
	}
	
}

void ATreasureChest::AuthorityInteract(APawn* InteractingPawn)
{
	if(!bUsed)
	{
		OnInteractedOnServer();
		bUsed=true;
	}
	
}

void ATreasureChest::OnInteractedOnServer_Implementation()
{

}
