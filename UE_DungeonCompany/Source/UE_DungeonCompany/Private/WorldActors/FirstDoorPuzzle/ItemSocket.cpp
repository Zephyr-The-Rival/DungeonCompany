// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/FirstDoorPuzzle/ItemSocket.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "Inventory/InventorySlot.h"
#include "Inventory/Inventory.h"
#include "UI/PlayerHud/PlayerHud.h"
#include "Items/ItemData.h"
#include "Items/WorldItem.h"

// Sets default values
AItemSocket::AItemSocket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->bReplicates = true;
	this->bInteractOnServer = false;
}

// Called when the game starts or when spawned
void AItemSocket::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemSocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemSocket::OnHovered(APlayerCharacter* PlayerCharacter)
{
	if (!IsValid(this->ItemToSacrifice))
	{
		PlayerCharacter->GetMyHud()->ShowTextInteractPrompt("ERROR! NO ITEM TO SACRIFICE DEFINED");
		return;
	}

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

void AItemSocket::Server_ItemPlaced_Implementation()
{
	OnItemPlaced();
}

void AItemSocket::OnItemPlaced_Implementation()
{
}

void AItemSocket::Interact(APawn* InteractingPawn)
{
	if (!IsValid(this->ItemToSacrifice))
		return;

	if (APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(InteractingPawn))
	{
		if (IsValid(playerCharacter->GetCurrentlyHeldInventorySlot()->MyItem) && playerCharacter->GetCurrentlyHeldInventorySlot()->MyItem->IsA(ItemToSacrifice))
		{
			playerCharacter->RemoveItemFromInventorySlot(playerCharacter->GetCurrentlyHeldInventorySlot());
			this->Server_ItemPlaced();
		}
	}		
}