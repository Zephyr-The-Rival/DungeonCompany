// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/MainMenuGM.h"
#include "Subsystems/SessionSubsystem.h"
#include "Subsystems/EOSSubsystem.h"

void AMainMenuGM::BeginPlay()
{
	Super::BeginPlay();

	GetGameInstance()->GetSubsystem<USessionSubsystem>()->DestroyCurrentSession();
	GetGameInstance()->GetSubsystem<UEOSSubsystem>()->Login();

}
