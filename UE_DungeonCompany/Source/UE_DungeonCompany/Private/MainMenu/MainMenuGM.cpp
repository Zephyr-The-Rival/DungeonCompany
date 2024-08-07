// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/MainMenuGM.h"
#include "GenericGame/MultiDevice_PC.h"
#include "Subsystems/SessionSubsystem.h"

AMainMenuGM::AMainMenuGM()
{
	PlayerControllerClass = AMultiDevice_PC::StaticClass();
}

void AMainMenuGM::BeginPlay()
{
	Super::BeginPlay();

	GetGameInstance()->GetSubsystem<USessionSubsystem>()->DestroyCurrentSession();

}
