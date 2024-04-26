// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Inventory.h"
#include "Inventory/InventorySlot.h"
#include "Items/ItemData.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;



	// ...
}


// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < this->NumInventorySlots; i++)
	{
		this->Slots.Add(NewObject<UInventorySlot>());
	}
	
}


// Called every frame
void UInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

}

bool UInventory::AddItem(UItemData* item)
{
	for (UInventorySlot* s : this->Slots)
	{		
		if (!IsValid(s->MyItem))//free slot
		{
			s->MyItem = item;
			return true;
		}			
	}
	return false;
}

