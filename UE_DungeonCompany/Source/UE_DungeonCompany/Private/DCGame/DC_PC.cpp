// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_PC.h"
#include "Net/VoiceConfig.h"
#include "UI/PlayerHud/PlayerHud.h"

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

	UVOIPStatics::SetMicThreshold(-3.0);

	GetWorld()->Exec(GetWorld(), TEXT("OSS.VoiceLoopback 1"));
	ToggleSpeaking(true);

	MyPlayerHud = CreateWidget<UPlayerHud>(this, PlayerHudClass);
	MyPlayerHud->AddToViewport();

}