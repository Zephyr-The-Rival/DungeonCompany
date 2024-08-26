// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Mushroom.h"

AMushroom::AMushroom()
{
	MushroomMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MushroomMesh"));
	RootComponent = MushroomMesh;

	MushroomMesh->SetCollisionProfileName(FName("WorldItem"));

	MushroomMesh->SetEnableGravity(true);
	MushroomMesh->SetSimulatePhysics(true);
}

void AMushroom::OnHoldingInHand_Implementation(bool LocallyControlled)
{
	Super::OnHoldingInHand_Implementation(LocallyControlled);

	MushroomMesh->SetEnableGravity(false);
	MushroomMesh->SetSimulatePhysics(false);
}
