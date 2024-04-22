// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/Ladder.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
ALadder::ALadder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));


	LadderMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("LadderMesh"));
	LadderMesh->SetupAttachment(RootComponent);

}

void ALadder::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	LadderMesh->ClearInstances();

	if(!LadderSectionReference)
		return;

	LadderMesh->SetStaticMesh(LadderSectionReference);
	LadderSectionReference->SetMaterial(0, Material);
	
	for (unsigned int i = 0; i < SectionsCount; ++i)
	{
		FVector translation = FVector::UpVector * (i * LadderSectionGap);
		LadderMesh->AddInstance(FTransform(translation), false);
	}

}

// Called when the game starts or when spawned
void ALadder::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALadder::Interact()
{
}

