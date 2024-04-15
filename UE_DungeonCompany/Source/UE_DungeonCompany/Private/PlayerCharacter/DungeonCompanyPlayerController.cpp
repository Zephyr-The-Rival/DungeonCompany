// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter/DungeonCompanyPlayerController.h"

void ADungeonCompanyPlayerController::BeginPlay()
{
	Super::BeginPlay();


	FInputModeGameOnly mode;
	this->SetInputMode(mode);

	
}