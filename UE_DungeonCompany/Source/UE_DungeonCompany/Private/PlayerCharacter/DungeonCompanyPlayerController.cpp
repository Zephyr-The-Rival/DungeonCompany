// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter/DungeonCompanyPlayerController.h"

void ADungeonCompanyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (this->HasAuthority())
	{
		
		FInputModeGameOnly mode;
		this->SetInputMode(mode);
		UE_LOG(LogTemp, Warning, TEXT("Custom Player Controller BeginPlay called"));
	}
	
}