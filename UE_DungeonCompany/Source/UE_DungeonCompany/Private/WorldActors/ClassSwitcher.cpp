// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/ClassSwitcher.h"

#include "DCGame/DC_PC.h"


// Sets default values
AClassSwitcher::AClassSwitcher()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bInteractOnServer=true;
}

// Called when the game starts or when spawned
void AClassSwitcher::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AClassSwitcher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AClassSwitcher::OnHovered(APlayerCharacter* PlayerCharacter)
{
	this->InteractPromptText="SwichToMercenary";
	Super::OnHovered(PlayerCharacter);
	
}

void AClassSwitcher::AuthorityInteract(APawn* InteractingPawn)
{
	Super::Interact(InteractingPawn);
	Cast<ADC_PC>(InteractingPawn->GetController())->SwitchPlayerCharacterClass(this->ClassToSwitchTo);
}

