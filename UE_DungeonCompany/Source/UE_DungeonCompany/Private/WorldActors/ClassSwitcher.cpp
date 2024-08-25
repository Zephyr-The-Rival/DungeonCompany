// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/ClassSwitcher.h"


// Sets default values
AClassSwitcher::AClassSwitcher()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
	Super::OnHovered(PlayerCharacter);
	this->InteractPromptText="SwichToMercenary";
}

