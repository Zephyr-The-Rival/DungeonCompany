// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericGame/MultiDevice_PC.h"

#include "EnhancedInputSubsystems.h"

void AMultiDevice_PC::OnAnyKeyPressed(const FKey& Key)
{
	if(bUsingGamepad == Key.IsGamepadKey())
		return;

	bUsingGamepad = Key.IsGamepadKey();

	OnInputDeviceChanged.Broadcast(bUsingGamepad);
}

void AMultiDevice_PC::UpdateMapping(FName MappingName, FKey NewKey)
{
	ULocalPlayer* localPlayer = GetLocalPlayer();

	if (!localPlayer)
		return;

	auto inputLocalPLayer = localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if(!inputLocalPLayer)
		return;

	inputLocalPLayer->AddPlayerMappedKeyInSlot(MappingName, NewKey);
	inputLocalPLayer->
}

void AMultiDevice_PC::SetupInputComponent()
{
	Super::SetupInputComponent();
	
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
