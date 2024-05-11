// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Inventory.h"
#include "Inventory/InventorySlot.h"
#include "Items/ItemData.h"
#include "DC_Statics.h"

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

int32 UInventory::AddItem(UItemData* Item)
{
	//returns the slot index or -1
	for (int32 i=0; i<NumInventorySlots;i++)
	{		
		if (!IsValid(Slots[i]->MyItem))//free slot
		{
			Slots[i]->MyItem = Item;
			return i;
		}			
	}
	return -1;
}

UItemData* UInventory::GetItemAtIndex(int32 Index)
{
	return Slots[Index]->MyItem;
}

void UInventory::RemoveItem(UItemData* ItemToRemove)
{
	for (UInventorySlot* s : this->Slots)
	{
		if (s->MyItem == ItemToRemove)
		{
			s->MyItem = nullptr;
			return;
		}		
	}
}

void UInventory::RemoveItem(int32 index)
{
	FString message = "removing item at index: ";
	message.AppendInt(index);
	LogWarning(*message);
	Slots[index]->MyItem = nullptr;
}

