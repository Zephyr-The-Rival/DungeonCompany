// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Puzzles/TimedInteractionPuzzle/PuzzleTrigger.h"
#include "Interactable.h"
#include "PuzzleButton.generated.h"

UCLASS()
class UE_DUNGEONCOMPANY_API APuzzleButton : public APuzzleTrigger, public IInteractable
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* ActivatedMaterial;

	UMaterialInterface* DefaultMaterial;
	
public:	
	APuzzleButton();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void Interact(APawn* InteractingPawn);
	virtual void AuthorityInteract(APawn* InteractingPawn);

	virtual void OnTriggerStarted_Implementation() override;

	virtual void OnTriggerStopped_Implementation() override;

};
