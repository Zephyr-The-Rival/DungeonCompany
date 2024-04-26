// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_PC.h"
#include "Net/VoiceConfig.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "UI/PlayerHud/PlayerHud.h"
#include "DC_Statics.h"

#include "Net/VoiceConfig.h"
#include "EnhancedInputComponent.h"

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

	ToggleSpeaking(!bPushToTalkActive);

	MyPlayerHud = CreateWidget<UPlayerHud>(this, PlayerHudClass);
	MyPlayerHud->MyCharacter = Cast<APlayerCharacter>(this->GetPawn());
	MyPlayerHud->AddToViewport();

}

void ADC_PC::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);

	if(!EIC)
		return;

	EIC->BindAction(PushToTalkAction, ETriggerEvent::Started, this, &ADC_PC::PushToTalkStarted);
	EIC->BindAction(PushToTalkAction, ETriggerEvent::Completed, this, &ADC_PC::PushToTalkCompleted);
}

void ADC_PC::PushToTalkStarted()
{
	if(!bPushToTalkActive)
		return;

	ToggleSpeaking(true);

}

void ADC_PC::PushToTalkCompleted()
{
	if(!bPushToTalkActive)
		return;

	ToggleSpeaking(false);

}

bool ADC_PC::IsPushToTalkActive() const
{
	return bPushToTalkActive;
}

void ADC_PC::SetPushToTalkActive(bool IsActive)
{
	bPushToTalkActive = IsActive;
}
