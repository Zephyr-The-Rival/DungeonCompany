// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_PC.h"
#include "Net/VoiceConfig.h"

ADC_PC::ADC_PC()
{

}

void ADC_PC::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly mode;
	this->SetInputMode(mode);

	if(!IsLocalController())
		return;

	UVOIPStatics::SetMicThreshold(-1.0);

	GetWorld()->Exec(GetWorld(), TEXT("OSS.VoiceLoopback 1"));
	ToggleSpeaking(true);

}