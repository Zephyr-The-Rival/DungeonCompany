// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "UObject/Object.h"
#include "TreasureChest.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UE_DUNGEONCOMPANY_API ATreasureChest : public AActor, public IInteractable
{
	GENERATED_BODY()

	ATreasureChest();

protected:

	virtual void OnHovered(APlayerCharacter* PlayerCharacter) override;
	virtual void AuthorityInteract(APawn* InteractingPawn) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnInteractedOnServer();
	void OnInteractedOnServer_Implementation();

	bool bUsed=false;
	

	
};
