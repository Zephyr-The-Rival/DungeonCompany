// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericGame/MultiDevice_PC.h"

#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"

void AMultiDevice_PC::OnAnyKeyPressed(const FKey& Key)
{
	if(bUsingGamepad == Key.IsGamepadKey())
		return;

	bUsingGamepad = Key.IsGamepadKey();

	OnInputDeviceChanged.Broadcast(bUsingGamepad);
}

UEnhancedInputLocalPlayerSubsystem* AMultiDevice_PC::GetInputLocalPlayer() const
{
	ULocalPlayer* localPlayer = GetLocalPlayer();

	if (!localPlayer)
		return nullptr;

	return localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
}

void AMultiDevice_PC::UpdateMapping(FName MappingName, FKey NewKey)
{
	auto inputLocalPLayer = GetInputLocalPlayer();

	if(!inputLocalPLayer)
		return;

	auto userSettings = inputLocalPLayer->GetUserSettings();
	
	FMapPlayerKeyArgs keyArgs;
	keyArgs.Slot = EPlayerMappableKeySlot::First;
	keyArgs.MappingName = MappingName;
	keyArgs.NewKey = NewKey;

	FGameplayTagContainer failureReason;
	
	userSettings->MapPlayerKey(keyArgs, failureReason);
	userSettings->ApplySettings();
	userSettings->SaveSettings();

}

FKey AMultiDevice_PC::GetCurrentKeyForMapping(FName MappingName) const
{
	auto inputLocalPLayer = GetInputLocalPlayer();          
                                                        
	if(!inputLocalPLayer)                                   
		return FKey();                                             
                                                        
	auto userSettings = inputLocalPLayer->GetUserSettings();
                                                        
	FMapPlayerKeyArgs keyArgs;                              
	keyArgs.Slot = EPlayerMappableKeySlot::First;           
	keyArgs.MappingName = MappingName;                      
	return userSettings->GetCurrentKeyProfile()->FindKeyMapping(keyArgs)->GetCurrentKey();

}

AMultiDevice_PC::AMultiDevice_PC()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> CharacterIMC(TEXT("/Game/_DungeonCompanyContent/Input/CharacterIMC"));
	AllMappings.Add(CharacterIMC.Object);

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> ControllerIMC(TEXT("/Game/_DungeonCompanyContent/Input/ControllerIMC"));
	AllMappings.Add(ControllerIMC.Object);

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InventoryIMC(TEXT("/Game/_DungeonCompanyContent/Input/InventoryIMC"));
	AllMappings.Add(InventoryIMC.Object);

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> PostMortemIMC(TEXT("/Game/_DungeonCompanyContent/Input/PostMortemIMC"));
	AllMappings.Add(PostMortemIMC.Object);
}

void AMultiDevice_PC::BeginPlay()
{
	Super::BeginPlay();

	auto inputLocalPlayer = GetInputLocalPlayer();

	if(!inputLocalPlayer)
		return;

	auto userSettings = inputLocalPlayer->GetUserSettings();
	int mappingsNum = AllMappings.Num();

	for(int i = 0; i < mappingsNum; ++i)
	{
		userSettings->RegisterInputMappingContext(AllMappings[i]);
	}
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
