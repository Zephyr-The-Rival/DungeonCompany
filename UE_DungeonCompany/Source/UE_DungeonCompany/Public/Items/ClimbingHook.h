// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/WorldItem.h"
#include "ClimbingHook.generated.h"

/**
 * 
 */
 class UCableComponent;

UCLASS()
class UE_DUNGEONCOMPANY_API AClimbingHook : public AWorldItem
{
	GENERATED_BODY()

private:
	AClimbingHook* PartnerHook;
	APlayerCharacter* CharacterHoldingPartnerHook;

	UCableComponent* Rope;

	bool bAttached = false;

public:
	AClimbingHook();

public:
	virtual void Interact(APawn* InteractingPawn) override;

	virtual void TriggerPrimaryAction_Implementation(APlayerCharacter* User) override;
	

	
};
