// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/WallGenerator.h"

#include "Engine/StaticMeshActor.h"


// Sets default values
AWallGenerator::AWallGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWallGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWallGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWallGenerator::SpawnBricks(TArray<UStaticMeshComponent*> Bricks, FString NameSuffix)
{
	for(UStaticMeshComponent* Brick : Bricks)
	{
		
		AStaticMeshActor* SMA =Cast<AStaticMeshActor>( GetWorld()->SpawnActor(AStaticMeshActor::StaticClass() ,&Brick->GetComponentTransform()));
		SMA->GetStaticMeshComponent()->SetStaticMesh(Brick->GetStaticMesh());
		//SMA->SetActorLabel("WallBrick"+ NameSuffix);
		//SMA->SetFolderPath("GeneratedBrickWalls");
	}
}

