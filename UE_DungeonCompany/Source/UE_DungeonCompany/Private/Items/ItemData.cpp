// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemData.h"
#include "Items/WorldItem.h"

//UItemData::UItemData()
//{
//
//}

AWorldItem* UItemData::SpawnMyItem(FTransform transform)
{
	return GetWorld()->SpawnActor<AWorldItem>(MyWorldItem, transform);
}
