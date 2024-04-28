// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventorySlot.generated.h"

class UItemData;
/**
 * 
 */
UCLASS(Blueprintable)
class UE_DUNGEONCOMPANY_API UInventorySlot : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UItemData* MyItem;
};
