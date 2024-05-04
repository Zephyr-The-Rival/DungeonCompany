// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_GM.h"
#include "DCGame/DC_PC.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "Subsystems/EOSSubsystem.h"

ADC_GM::ADC_GM()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = ADC_PC::StaticClass();

}

void ADC_GM::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GetGameInstance()->GetSubsystem<UEOSSubsystem>()->RegisterPlayer(NewPlayer))
		UE_LOG(LogTemp, Log, TEXT("Registration successful"))
	else
		UE_LOG(LogTemp, Warning, TEXT("Registration failed!"));

}
