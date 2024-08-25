// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_PS.h"

#include "Subsystems/VoiceChatSubsystem.h"

void ADC_PS::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* pc = GetPlayerController(); pc && pc->IsLocalController())
		return;

	GetGameInstance()->GetSubsystem<UVoiceChatSubsystem>()->RegisterPlayerState(this);
}

void ADC_PS::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (APlayerController* pc = GetPlayerController(); pc && pc->IsLocalController())
		return;

	GetGameInstance()->GetSubsystem<UVoiceChatSubsystem>()->UnregisterPlayerState(this);
	
	Super::EndPlay(EndPlayReason);
}
