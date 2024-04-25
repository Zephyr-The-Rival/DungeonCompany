// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_GM.h"
#include "DCGame/DC_PC.h"
#include "PlayerCharacter/PlayerCharacter.h"

ADC_GM::ADC_GM()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = ADC_PC::StaticClass();

}
