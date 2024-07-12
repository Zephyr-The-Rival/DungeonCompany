// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/BasicButton.h"
#include "DC_Statics.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "UI/PlayerHud/PlayerHud.h"

// Sets default values
ABasicButton::ABasicButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->bInteractOnServer = true;

}

// Called when the game starts or when spawned
void ABasicButton::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasicButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABasicButton::AuthorityInteract(APawn* InteractingPawn)
{
	this->OnButtonPressed.Broadcast(InteractingPawn);
}

void ABasicButton::OnHovered(APlayerCharacter* PlayerCharacter)
{
	PlayerCharacter->GetMyHud()->ShowTextInteractPrompt(InteractPromptText);
}


