// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/FirstDoorPuzzle/ItemAltar.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "Inventory/InventorySlot.h"
#include "Inventory/Inventory.h"
#include "UI/PlayerHud/PlayerHud.h"
#include "Items/ItemData.h"
#include "Items/WorldItem.h"

// Sets default values
AItemAltar::AItemAltar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->bReplicates = true;
	this->bInteractOnServer = true;
}

// Called when the game starts or when spawned
void AItemAltar::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemAltar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemAltar::OnHovered(APlayerCharacter* PlayerCharacter)
{

	if (IsValid(PlayerCharacter->GetCurrentlyHeldInventorySlot()->MyItem))
	{
		if (PlayerCharacter->GetCurrentlyHeldInventorySlot()->MyItem->IsA(ItemToSacrifice))
		{
			PlayerCharacter->GetMyHud()->ShowTextInteractPrompt("InsertCrystal");
			return;
		}
	}
		PlayerCharacter->GetMyHud()->ShowTextInteractPrompt("Can't Interact");
}

void AItemAltar::AuthorityInteract(APawn* InteractingPawn)
{
	if (APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(InteractingPawn))
	{
		if (IsValid(playerCharacter->GetCurrentlyHeldInventorySlot()->MyItem))
		{
			if (playerCharacter->GetCurrentlyHeldInventorySlot()->MyItem->IsA(ItemToSacrifice))
				playerCharacter->RemoveItemFromInventorySlot(playerCharacter->GetCurrentlyHeldInventorySlot());
		}		
	}		
}