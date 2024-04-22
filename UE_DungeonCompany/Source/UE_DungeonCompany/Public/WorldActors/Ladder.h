// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Ladder.generated.h"

class UBoxComponent;

UCLASS()
class UE_DUNGEONCOMPANY_API ALadder : public AActor, public IInteractable
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UStaticMesh* LadderSectionReference;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;

	UInstancedStaticMeshComponent* LadderMesh;

	UBoxComponent* InteractVolume;

	UPROPERTY(EditAnywhere, Category = ProceduralGeneration)
	unsigned int SectionsCount = 1;

	UPROPERTY(EditAnywhere, Category = ProceduralGeneration)
	float SectionHeight = 100.f;

	UPROPERTY(EditAnywhere, Category = ProceduralGeneration)
	bool bSectionOriginInMid = false;

	UPROPERTY(EditAnywhere, Category = Interaction)
	FVector2D InteractionArea = FVector2D(10, 20);

	
public:	
	ALadder();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

public:
	virtual void Interact(APawn* InteractingPawn) override;

	UFUNCTION()
	void OnInteractVolumeEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractVolumeLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
