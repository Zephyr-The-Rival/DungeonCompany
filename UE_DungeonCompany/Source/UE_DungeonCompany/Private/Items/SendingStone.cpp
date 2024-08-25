// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/SendingStone.h"

#include "PlayerCharacter/PlayerCharacter.h"


// Sets default values
ASendingStone::ASendingStone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASendingStone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASendingStone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASendingStone::TriggerLocalPrimaryAction_Implementation(APlayerCharacter* User)
{
	Super::TriggerLocalPrimaryAction_Implementation(User);
	User->AttackBlend=1;
	User->bSwitchHandAllowed=false;
	User->bIsUsingSendingStone=true;
	this->ToggleTutorial(true);
}

void ASendingStone::EndLocalPrimaryActionHold_Implementation(APlayerCharacter* User)
{
	Super::EndLocalPrimaryActionHold_Implementation(User);
	User->AttackBlend=0;
	User->bSwitchHandAllowed=true;
	User->bIsUsingSendingStone=false;
	this->ToggleTutorial(false);
}

void ASendingStone::Server_SendSignal_Implementation(ESendingStoneAnimatoinState Signal)
{
}

void ASendingStone::ToggleTutorial_Implementation(bool bOn)
{
}



