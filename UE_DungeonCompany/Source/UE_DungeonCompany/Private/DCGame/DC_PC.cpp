// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_PC.h"
#include "Net/VoiceConfig.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "UI/PlayerHud/PlayerHud.h"
#include "DC_Statics.h"
#include "DCGame/DC_PostMortemPawn.h"
#include "DCGame/DC_GM.h"

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

void ADC_PC::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(!InPawn)
		return;

	UClass* pawnClass = InPawn->StaticClass();

	if(InPawn->IsA(APlayerCharacter::StaticClass()))
		SetPawnType(EPawnType::Gameplay);
	else if (InPawn->IsA(ADC_PostMortemPawn::StaticClass()))
		SetPawnType(EPawnType::Spectator);
	else
		SetPawnType(EPawnType::None);
}

void ADC_PC::OnUnPossess()
{
	SetPawnType(EPawnType::None);
}

void ADC_PC::SetPawnType(EPawnType NewPawnType)
{
	if(NewPawnType == PawnType)
		return;

	PawnType = NewPawnType;
	OnPawnTypeChanged(NewPawnType);
	EventOnPawnTypeChanged.Broadcast(NewPawnType);
}

void ADC_PC::OnPawnTypeChanged_Implementation(EPawnType NewPawnType)
{

}

void ADC_PC::SetGamePadAccelerationSpeed(float InSpeed)
{
	GamepadAccelerationSpeed = InSpeed;
}

void ADC_PC::SetLastLookVectorLength(float InLength)
{
	LastLookVectorLength = InLength;
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

	FInputKeyBinding keysIKB(FInputChord(EKeys::AnyKey, false, false, false, false), EInputEvent::IE_Pressed);

	keysIKB.bConsumeInput = true;
	keysIKB.bExecuteWhenPaused = false;

	keysIKB.KeyDelegate.GetDelegateWithKeyForManualSet().BindLambda([this](const FKey& Key) {
		OnAnyKeyPressed(Key);
	});

	InputComponent->KeyBindings.Add(keysIKB);

	FInputKeyBinding mouseIKB(FInputChord(EKeys::Mouse2D, false, false, false, false), EInputEvent::IE_Pressed);

	mouseIKB.bConsumeInput = true;
	mouseIKB.bExecuteWhenPaused = false;

	mouseIKB.KeyDelegate.GetDelegateWithKeyForManualSet().BindLambda([this](const FKey& Key) {
		OnAnyKeyPressed(Key);
	});

	InputComponent->KeyBindings.Add(mouseIKB);

}

void ADC_PC::ToggleOptions()
{
	bOptionsMenuIsOn = !bOptionsMenuIsOn;


	APlayerCharacter* playerCharacter = GetPawn<APlayerCharacter>();

	ToggleOptionsMenu(bOptionsMenuIsOn);
	if(!playerCharacter)
		return;
	
	if(bOptionsMenuIsOn)
		playerCharacter->DeactivateCharacterInputMappings();
	else
		playerCharacter->ActivateCharacterInputMappings();
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

void ADC_PC::OnAnyKeyPressed(const FKey& Key)
{
	if(bUsingGamepad == Key.IsGamepadKey())
		return;

	bUsingGamepad = Key.IsGamepadKey();

	OnInputDeviceChanged.Broadcast(bUsingGamepad);

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

void ADC_PC::Server_RequestRespawn_Implementation()
{
	GetWorld()->GetAuthGameMode<ADC_GM>()->Respawn(this);
}

void ADC_PC::ToggleOptionsMenu_Implementation(bool On)
{
	LogWarning(TEXT("OverrideMe"));
}

