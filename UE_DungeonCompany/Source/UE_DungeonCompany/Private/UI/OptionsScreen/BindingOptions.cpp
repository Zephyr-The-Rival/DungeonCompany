// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OptionsScreen/BindingOptions.h"
#include "GenericGame/MultiDevice_PC.h"
#include "UI/OptionsScreen/BindingOptionsItem.h"

#include "InputMappingContext.h"
#include "PlayerMappableKeySettings.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"

void UBindingOptions::NativeConstruct()
{
	Super::NativeConstruct();

	AMultiDevice_PC* owner = GetOwningPlayer<AMultiDevice_PC>();

	if(!BOItemClass || !owner)
		return;

	auto allMappings = owner->GetAllMappingContexts();
	int mappingsNum = allMappings.Num();

	for(int i = 0; i < mappingsNum; ++i)
	{
		CreateBindingsForMappingContext(allMappings[i]);
	}
	
}

void UBindingOptions::CreateBindingsForMappingContext(UInputMappingContext* BindingMappingContext)
{
	const TArray<FEnhancedActionKeyMapping>& allKeyMappings = BindingMappingContext->GetMappings();

	int keyMappingsNum = allKeyMappings.Num();

	for(int i = 0; i < keyMappingsNum; ++i)
	{
		auto keyMapping = allKeyMappings[i];
		if(!keyMapping.GetPlayerMappableKeySettings() || keyMapping.Key.IsGamepadKey() != bBindGamepad)
			continue;

		UBindingOptionsItem* item = WidgetTree->ConstructWidget<UBindingOptionsItem>(BOItemClass);

		if(!item)
			continue;
		
		item->SetMappableKey(keyMapping);
		item->SetMappingContext(BindingMappingContext);

		item->SetAllowGamepadKeys(bBindGamepad);

		FString category = keyMapping.GetPlayerMappableKeySettings()->DisplayCategory.ToString();

		if(category.Contains("Movement"))
			MovementContainer->AddChild(item);
		else
			GeneralContainer->AddChild(item);
		
		BindingOptionsItems.Add(item);
	}
}

void UBindingOptions::ResetMappings()
{
	int itemsNum = BindingOptionsItems.Num();

	for(int i = 0; i < itemsNum; ++i)
	{
		auto currentItem = BindingOptionsItems[i];
		if(!IsValid(currentItem))
			continue;

		currentItem->ResetMapping();
	}
}
