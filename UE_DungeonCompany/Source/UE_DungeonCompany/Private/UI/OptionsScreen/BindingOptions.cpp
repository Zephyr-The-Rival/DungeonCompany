// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OptionsScreen/BindingOptions.h"

#include "InputMappingContext.h"
#include "PlayerMappableKeySettings.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "UI/OptionsScreen/BindingOptionsItem.h"

void UBindingOptions::NativeConstruct()
{
	Super::NativeConstruct();

	if(!BOItemClass)
		return;
	
	int mappingsNum = AllMappings.Num();

	for(int i = 0; i < mappingsNum; ++i)
	{
		CreateBindingsForMappingContext(AllMappings[i]);
	}
	
}

void UBindingOptions::CreateBindingsForMappingContext(UInputMappingContext* BindingMappingContext)
{
	const TArray<FEnhancedActionKeyMapping>& allKeyMappings = BindingMappingContext->GetMappings();

	int keyMappingsNum = allKeyMappings.Num();

	for(int i = 0; i < keyMappingsNum; ++i)
	{
		auto keyMapping = allKeyMappings[i];
		if(!keyMapping.GetPlayerMappableKeySettings())
			continue;

		UBindingOptionsItem* item = WidgetTree->ConstructWidget<UBindingOptionsItem>(BOItemClass);
		item->SetMappableKey(keyMapping);
		item->SetMappingContext(BindingMappingContext);

		if(keyMapping.Key.IsGamepadKey())
			ControllerContainer->AddChild(item);
		else
			MKContainer->AddChild(item);
	}
}
