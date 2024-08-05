// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OptionsScreen/BindingOptionsItem.h"

#include "InputAction.h"
#include "InputMappingContext.h"
#include "Components/InputKeySelector.h"
#include "PlayerMappableKeySettings.h"
#include "Components/TextBlock.h"

void UBindingOptionsItem::SetMappableKey(const FEnhancedActionKeyMapping& InMappableKey)
{
	MappableKey = InMappableKey;
}

void UBindingOptionsItem::SetMappingContext(UInputMappingContext* InMappingContext)
{
	MappingContext = InMappingContext;
}

void UBindingOptionsItem::NativeConstruct()
{
	Super::NativeConstruct();

	KeySelector->OnKeySelected.AddDynamic(this, &UBindingOptionsItem::OnKeySelected);
	KeySelector->OnIsSelectingKeyChanged.AddDynamic(this, &UBindingOptionsItem::OnIsSelectingKeyChanged);

	if(!MappableKey.GetPlayerMappableKeySettings())
		return;
	
	KeyText->SetText(MappableKey.GetPlayerMappableKeySettings()->DisplayName);
	KeySelector->SetSelectedKey(MappableKey.Key);
	
}

void UBindingOptionsItem::OnKeySelected(FInputChord SelectedKey)
{
	MappableKey.GetPlayerMappableKeySettings()->Name
	MappingContext->Add
}

void UBindingOptionsItem::OnIsSelectingKeyChanged()
{
	
}
