// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/ItemData.h"
#include "Torch_Data.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UE_DUNGEONCOMPANY_API UTorch_Data : public UItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOn=false;
};
