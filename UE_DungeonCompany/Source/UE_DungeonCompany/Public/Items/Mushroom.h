// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/WorldItem.h"
#include "Mushroom.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API AMushroom : public AWorldItem
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* MushroomMesh;

public:
	AMushroom();

	virtual void OnHoldingInHand_Implementation(bool LocallyControlled) override;
};
