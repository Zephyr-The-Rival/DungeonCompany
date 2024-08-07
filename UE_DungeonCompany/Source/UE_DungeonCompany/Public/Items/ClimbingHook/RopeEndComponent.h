// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Interactable.h"
#include "RopeEndComponent.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API URopeEndComponent : public UStaticMeshComponent, public IInteractable
{
	GENERATED_BODY()

public:
	URopeEndComponent();

public:
	virtual void AuthorityInteract(APawn* InteractingPawn);

	void Throw(FVector Direction, float Strength);

	
	
};
