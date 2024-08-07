// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericGame/MultiDevice_PC.h"

#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"
#include "PlayerMappableKeySettings.h"

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

void AMultiDevice_PC::UpdateMapping(FName MappingName, FKey NewKey) const
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

	ActionKeyCache.Empty();
	OnKeyMappingChanged.Broadcast();
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

	if(!userSettings || !userSettings->GetCurrentKeyProfile() || !userSettings->GetCurrentKeyProfile()->FindKeyMapping(keyArgs))
		return FKey();
	
	return userSettings->GetCurrentKeyProfile()->FindKeyMapping(keyArgs)->GetCurrentKey();
}

const TTuple<TArray<FCustomMapping>, TArray<FCustomMapping>>& AMultiDevice_PC::GetCurrentKeysForAction(UInputAction* InputAction) const
{
	if(ActionKeyCache.Contains(InputAction))
		return ActionKeyCache[InputAction];
	
	int contextsNum = AllMappingContexts.Num();

	TArray<FEnhancedActionKeyMapping> foundMKMapping;
	TArray<FEnhancedActionKeyMapping> foundControllerMapping;

	for(int i = 0; i < contextsNum; ++i)
	{
		auto allMappings = AllMappingContexts[i]->GetMappings();

		int mappingsNum = allMappings.Num();

		for(int j = 0; j < mappingsNum; ++j)
		{
			auto currentMapping = allMappings[j];
			if(currentMapping.Action != InputAction)
				continue;

			FKey currentKey = currentMapping.Key;

			if(currentKey.IsGamepadKey())
			{
				foundControllerMapping.Add(currentMapping);
			} else if(!currentKey.IsGamepadKey())
			{
				foundMKMapping.Add(currentMapping);
			}
		}
	}
	
	ActionKeyCache.Add(InputAction);

	int mkNum = foundMKMapping.Num();
	
	for(int i = 0; i < mkNum; ++i)
	{
		auto currentMapping = foundMKMapping[i];
		
		ActionKeyCache[InputAction].Get<0>().Add({GetCurrentKeyForMapping(currentMapping.GetMappingName()), currentMapping.Modifiers});
	}

	int controllerNum = foundControllerMapping.Num();

	for(int i = 0; i < controllerNum; ++i)
	{
		auto currentMapping = foundControllerMapping[i];

		ActionKeyCache[InputAction].Get<1>().Add({GetCurrentKeyForMapping(currentMapping.GetMappingName()), currentMapping.Modifiers});
	}
	
	return ActionKeyCache[InputAction];
}

const TArray<FCustomMapping>& AMultiDevice_PC::GetKeyboardKeysForAction(UInputAction* InputAction) const
{
	return GetCurrentKeysForAction(InputAction).Get<0>();
}

const TArray<FCustomMapping>& AMultiDevice_PC::GetGamepadKeysForAction(UInputAction* InputAction) const
{
	return GetCurrentKeysForAction(InputAction).Get<1>();}

AMultiDevice_PC::AMultiDevice_PC()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> CharacterIMC(TEXT("/Game/_DungeonCompanyContent/Input/CharacterIMC"));
	AllMappingContexts.Add(CharacterIMC.Object);

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> ControllerIMC(TEXT("/Game/_DungeonCompanyContent/Input/ControllerIMC"));
	AllMappingContexts.Add(ControllerIMC.Object);

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InventoryIMC(TEXT("/Game/_DungeonCompanyContent/Input/InventoryIMC"));
	AllMappingContexts.Add(InventoryIMC.Object);

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> PostMortemIMC(TEXT("/Game/_DungeonCompanyContent/Input/PostMortemIMC"));
	AllMappingContexts.Add(PostMortemIMC.Object);
}

void AMultiDevice_PC::BeginPlay()
{
	Super::BeginPlay();

	auto inputLocalPlayer = GetInputLocalPlayer();

	if(!inputLocalPlayer)
		return;

	auto userSettings = inputLocalPlayer->GetUserSettings();
	int mappingsNum = AllMappingContexts.Num();

	for(int i = 0; i < mappingsNum; ++i)
	{
		userSettings->RegisterInputMappingContext(AllMappingContexts[i]);
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
