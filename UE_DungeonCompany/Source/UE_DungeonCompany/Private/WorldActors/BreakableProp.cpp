// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/BreakableProp.h"

// Sets default values
ABreakableProp::ABreakableProp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABreakableProp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABreakableProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableProp::Hit_Implementation(FVector ImpulseDirection, TSubclassOf<AWeapon> WeaponClass)
{
	
}

