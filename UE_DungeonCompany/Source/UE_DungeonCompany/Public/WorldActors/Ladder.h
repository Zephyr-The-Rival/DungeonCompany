// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Ladder.generated.h"

UCLASS()
class UE_DUNGEONCOMPANY_API ALadder : public AActor, public IInteractable
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UStaticMesh* LadderSectionReference;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;

	UMaterialInstanceDynamic* DynMaterial;

	UPROPERTY(EditAnywhere)
	unsigned int SectionsCount = 1;

	UPROPERTY(EditAnywhere)
	float LadderSectionGap = 100.f;

	UInstancedStaticMeshComponent* LadderMesh;
	
public:	
	ALadder();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

public:
	virtual void Interact() override;

};
