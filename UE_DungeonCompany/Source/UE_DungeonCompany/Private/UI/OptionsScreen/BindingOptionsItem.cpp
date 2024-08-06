// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OptionsScreen/BindingOptionsItem.h"
#include "GenericGame/MultiDevice_PC.h"

#include "InputMappingContext.h"
#include "Components/InputKeySelector.h"
#include "PlayerMappableKeySettings.h"
#include "Components/TextBlock.h"
#include "Framework/Application/NavigationConfig.h"

void UBindingOptionsItem::SetMappableKey(const FEnhancedActionKeyMapping& InMappableKey)
{
	MappableKey = InMappableKey;
}

void UBindingOptionsItem::SetMappingContext(UInputMappingContext* InMappingContext)
{
	MappingContext = InMappingContext;
}

void UBindingOptionsItem::SetAllowGamepadKeys(bool bGamepadKeysAllowed)
{
	KeySelector->SetAllowGamepadKeys(bGamepadKeysAllowed);
}

void UBindingOptionsItem::ResetMapping()
{
	bIsSelectingNewKey = true;
	KeySelector->SetSelectedKey(MappableKey.Key);
}

void UBindingOptionsItem::NativeConstruct()
{
	Super::NativeConstruct();

	KeySelector->OnKeySelected.AddDynamic(this, &UBindingOptionsItem::OnKeySelected);
	KeySelector->OnIsSelectingKeyChanged.AddDynamic(this, &UBindingOptionsItem::OnIsSelectingKeyChanged);

	AMultiDevice_PC* owner = GetOwningPlayer<AMultiDevice_PC>();

	if(!MappableKey.GetPlayerMappableKeySettings() || !owner)
		return;
	
	KeyText->SetText(MappableKey.GetPlayerMappableKeySettings()->DisplayName);
	KeySelector->SetSelectedKey(owner->GetCurrentKeyForMapping(MappableKey.GetPlayerMappableKeySettings()->Name));

	
}

void UBindingOptionsItem::OnKeySelected(FInputChord SelectedKey)
{
	FKey tempOldKey = KeySelector->GetSelectedKey().Key;
	FKey newKey = SelectedKey.Key;
	
	if(!bIsSelectingNewKey)
		return;

	if(newKey.IsGamepadKey() != tempOldKey.IsGamepadKey())
		KeySelector->SetSelectedKey(tempOldKey);

	FString keyName = newKey.GetFName().ToString();
	
	if(keyName.Contains("LeftStick") )
		newKey = EKeys::Gamepad_Left2D;
	else if(keyName.Contains("RightStick"))
		newKey = EKeys::Gamepad_Right2D;
	
	OldKey = tempOldKey;

	AMultiDevice_PC* owner = GetOwningPlayer<AMultiDevice_PC>();

	if(!owner)
		return;
	
	owner->UpdateMapping(MappableKey.GetPlayerMappableKeySettings()->Name, newKey);
}

void UBindingOptionsItem::OnIsSelectingKeyChanged()
{
	if(bIsSelectingNewKey)
	{
		bIsSelectingNewKey = false;
		FSlateApplication::Get().GetNavigationConfig()->bAnalogNavigation = true;
	}
	else
	{
		bIsSelectingNewKey = true;
		FSlateApplication::Get().GetNavigationConfig()->bAnalogNavigation = false;
	}
	bIsSelectingNewKey = true;
}
