// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/LostItemsCollection.h"
#include "Items/ItemData.h"

ALostItemsCollection::ALostItemsCollection()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALostItemsCollection::AddLostItem(UItemData* InLostItem)
{
	if(!IsValid(InLostItem))
		return;
	
	LostItems.Add({InLostItem->GetClass(), InLostItem->SerializeMyData()});
}
