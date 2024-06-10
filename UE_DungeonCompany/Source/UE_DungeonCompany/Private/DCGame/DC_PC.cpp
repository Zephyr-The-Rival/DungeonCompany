// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_PC.h"
#include "Net/VoiceConfig.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "UI/PlayerHud/PlayerHud.h"
#include "DC_Statics.h"

#include "Net/VoiceConfig.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ADC_PC::ADC_PC()
{

}

void ADC_PC::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());

	if(!IsLocalController())
		return;

	UVOIPStatics::SetMicThreshold(-3.0);

	GetWorld()->Exec(GetWorld(), TEXT("OSS.VoiceLoopback 1"));

	ToggleSpeaking(true);
	if(bPushToTalkActive)
		ToggleSpeaking(false);

	MyPlayerHud = CreateWidget<UPlayerHud>(this, PlayerHudClass);
	MyPlayerHud->MyCharacter = Cast<APlayerCharacter>(this->GetPawn());
	MyPlayerHud->AddToViewport();

	if (!InputMapping)
		return;

	ULocalPlayer* localPlayer = GetLocalPlayer();

	if (!localPlayer)
		return;

	UEnhancedInputLocalPlayerSubsystem* inputSystem = localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (!inputSystem)
		return;

	inputSystem->AddMappingContext(InputMapping, 1);

}

void ADC_PC::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);

	if(!EIC)
		return;

	EIC->BindAction(OptionsAction, ETriggerEvent::Started, this, &ADC_PC::ToggleOptions);

	EIC->BindAction(PushToTalkAction, ETriggerEvent::Started, this, &ADC_PC::PushToTalkStarted);
	EIC->BindAction(PushToTalkAction, ETriggerEvent::Completed, this, &ADC_PC::PushToTalkCompleted);

}

void ADC_PC::ToggleOptions()
{
	bOptionsMenuIsOn = !bOptionsMenuIsOn;
	GetMyPlayerHud()->ToggleOptionsMenu(bOptionsMenuIsOn);

	APlayerCharacter* playerCharacter = GetPawn<APlayerCharacter>();

	ULocalPlayer* localPlayer = GetLocalPlayer();

	if (!playerCharacter || !localPlayer)
		return;

	UEnhancedInputLocalPlayerSubsystem* inputSystem = localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (!inputSystem)
		return;

	if(bOptionsMenuIsOn)
		inputSystem->RemoveMappingContext(playerCharacter->GetInputMapping());
	else
		inputSystem->AddMappingContext(playerCharacter->GetInputMapping(), 0);
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
	ToggleSpeaking(!IsActive);
	bPushToTalkActive = IsActive;
}
