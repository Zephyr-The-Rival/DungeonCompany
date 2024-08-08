// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldActors/Ladder.h"
#include "GeneratedLadder.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API AGeneratedLadder : public ALadder
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UStaticMesh* LadderSectionReference;

	UPROPERTY()
	UInstancedStaticMeshComponent* LadderMesh;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, Category = ProceduralGeneration)
	unsigned int SectionsCount = 1;

	UPROPERTY(EditAnywhere, Category = ProceduralGeneration)
	float SectionHeight = 100.f;

	UPROPERTY(EditAnywhere, Category = ProceduralGeneration)
	float SectionDepth = 20.f;

	UPROPERTY(EditAnywhere, Category = ProceduralGeneration)
	bool bSectionOriginInMid = false;

public:
	AGeneratedLadder();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	
};
