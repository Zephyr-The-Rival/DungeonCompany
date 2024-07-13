// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "ItemAltar.generated.h"

class APlayerCharacter;

UCLASS()
class UE_DUNGEONCOMPANY_API AItemAltar : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemAltar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	//interact stuff
	//virtual void Interact(APawn* InteractingPawn) override;
	virtual void AuthorityInteract(APawn* InteractingPawn) override;

	virtual void OnHovered(APlayerCharacter* PlayerCharacter) override;

};
