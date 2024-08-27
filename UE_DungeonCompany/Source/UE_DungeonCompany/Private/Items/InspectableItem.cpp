// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/InspectableItem.h"

#include "PlayerCharacter/PlayerCharacter.h"


// Sets default values
AInspectableItem::AInspectableItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AInspectableItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInspectableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInspectableItem::TriggerPrimaryActionHold_Implementation(APlayerCharacter* User)
{
	Super::TriggerPrimaryActionHold_Implementation(User);
	APlayerCharacter* MyCharacter= Cast<APlayerCharacter>(User);
	MyCharacter->SetAttackBlend(1);
	MyCharacter->bSwitchHandAllowed=false;
}

void AInspectableItem::EndPrimaryActionHold_Implementation(APlayerCharacter* User)
{
	Super::EndPrimaryActionHold_Implementation(User);
	APlayerCharacter* MyCharacter= Cast<APlayerCharacter>(User);

	MyCharacter->SetAttackBlend(0);
	MyCharacter->bSwitchHandAllowed=true;
}

