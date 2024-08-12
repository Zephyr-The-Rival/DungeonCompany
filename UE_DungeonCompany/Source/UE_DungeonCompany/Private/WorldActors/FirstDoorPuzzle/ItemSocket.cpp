// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/FirstDoorPuzzle/ItemSocket.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "Inventory/InventorySlot.h"
#include "Inventory/Inventory.h"
#include "UI/PlayerHud/PlayerHud.h"
#include "Items/ItemData.h"
#include "Items/WorldItem.h"
#include "Net/UnrealNetwork.h"

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

	if (IsValid(PlayerCharacter->GetCurrentlyHeldInventorySlot()->MyItem) && this->bPlayerCanPlaceItem && PlayerCharacter->GetCurrentlyHeldInventorySlot()->MyItem->IsA(this->ItemToSacrifice))
	{
		PlayerCharacter->GetMyHud()->ShowTextInteractPrompt(this->PromptOnCorrectItemHovered);
		return;
	}
	PlayerCharacter->GetMyHud()->ShowTextInteractPrompt(this->PromptNotUseable);
}




void AItemSocket::Interact(APawn* InteractingPawn)
{
	if (!IsValid(this->ItemToSacrifice) || !this->bPlayerCanPlaceItem)
		return;

	if (APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(InteractingPawn))
	{
		if (IsValid(playerCharacter->GetCurrentlyHeldInventorySlot()->MyItem) && playerCharacter->GetCurrentlyHeldInventorySlot()->MyItem->IsA(ItemToSacrifice))
		{
			playerCharacter->RemoveItemFromInventorySlot(playerCharacter->GetCurrentlyHeldInventorySlot());
			playerCharacter->PlaceItemOnSocket(this);
			playerCharacter->ResetInteractPrompt();
			if (this->bOneTimeUse)
				this->bPlayerCanPlaceItem = false;
			
		}
	}		
}

void AItemSocket::OnItemPlaced_Implementation()
{

}

void AItemSocket::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemSocket, bPlayerCanPlaceItem);
}
