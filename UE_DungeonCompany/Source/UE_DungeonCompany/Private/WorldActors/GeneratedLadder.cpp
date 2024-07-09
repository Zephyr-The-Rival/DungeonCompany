// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/GeneratedLadder.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/BoxComponent.h"

AGeneratedLadder::AGeneratedLadder()
{
	LadderMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("LadderMesh"));
	LadderMesh->SetupAttachment(RootComponent);
}

void AGeneratedLadder::OnConstruction(const FTransform& Transform)
{
	SetHeight((SectionHeight) * SectionsCount);

	Super::OnConstruction(Transform);
	LadderMesh->ClearInstances();

	LadderMesh->SetStaticMesh(LadderSectionReference);

	if (Material)
		LadderMesh->SetMaterial(0, Material);

	for (unsigned int i = 0; i < SectionsCount; ++i)
	{
		FVector translation = FVector::UpVector * (i * SectionHeight);
		LadderMesh->AddInstance(FTransform(translation), false);
	}

	LadderMesh->SetRelativeLocation(FVector(-SectionDepth, 0, bSectionOriginInMid * (SectionHeight / 2)));



}

