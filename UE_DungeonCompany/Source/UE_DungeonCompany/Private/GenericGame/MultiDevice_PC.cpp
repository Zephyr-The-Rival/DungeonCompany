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

FKey AMultiDevice_PC::GetCurrentKeyForAction(UInputAction* InputAction) const
{
	int64 key = bUsingGamepad? -(int64)InputAction : (int64)InputAction;
	
	if(ActionKeyCache.Contains(key))
		return ActionKeyCache[key];
	
	int contextsNum = AllMappingContexts.Num();

	FEnhancedActionKeyMapping foundMapping;
	bool bFoundMapping = false;

	for(int i = 0; i < contextsNum && !bFoundMapping; ++i)
	{
		auto allMappings = AllMappingContexts[i]->GetMappings();

		int mappingsNum = allMappings.Num();

		for(int j = 0; j < mappingsNum && !bFoundMapping; ++j)
		{
			if(allMappings[j].Action == InputAction && allMappings[j].Key.IsGamepadKey() == bUsingGamepad)
			{
				foundMapping = allMappings[j];
				bFoundMapping = true;
			}
		}
	}

	if(!bFoundMapping)
		return FKey();

	FKey result = GetCurrentKeyForMapping(foundMapping.GetMappingName());
	ActionKeyCache.Add(key, result);
	
	return GetCurrentKeyForMapping(foundMapping.GetMappingName());
}

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
