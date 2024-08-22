// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Potion.h"

#include "PlayerCharacter/PlayerCharacter.h"


// Sets default values
APotion::APotion()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APotion::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APotion::Local_ApplyEffect_Implementation(APlayerCharacter* User)
{
}

void APotion::TriggerPrimaryAction_Implementation(APlayerCharacter* User)
{
	User->StartDrinkingPotion();
	Super::TriggerPrimaryAction_Implementation(User);

}



