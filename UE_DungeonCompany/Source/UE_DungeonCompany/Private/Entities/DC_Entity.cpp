// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/DC_Entity.h"

// Sets default values
ADC_Entity::ADC_Entity()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADC_Entity::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADC_Entity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADC_Entity::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

