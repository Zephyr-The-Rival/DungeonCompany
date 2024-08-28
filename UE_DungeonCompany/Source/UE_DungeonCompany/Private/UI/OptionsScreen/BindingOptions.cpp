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

	if (!BOItemClass || !owner)
		return;

	RemoveAllChildrenOfContainer(CharacterGeneralContainer);
	RemoveAllChildrenOfContainer(CharacterMovementContainer);
	RemoveAllChildrenOfContainer(InventoryContainer);
	RemoveAllChildrenOfContainer(SpectatorContainer);
	RemoveAllChildrenOfContainer(GlobalContainer);

	auto allMappings = owner->GetAllMappingContexts();
	int mappingsNum = allMappings.Num();

	if (AMultiDevice_PC* pc = GetWorld()->GetFirstPlayerController<AMultiDevice_PC>(); pc && bAutoDetectWhatToBind)
		bBindGamepad = pc->IsUsingGamepad();

	for (int i = 0; i < mappingsNum; ++i)
	{
		CreateBindingsForMappingContext(allMappings[i]);
	}
}

void UBindingOptions::RemoveAllChildrenOfContainer(UVerticalBox* InContainer)
{
	const TArray<UWidget*>& allChildren = InContainer->GetAllChildren();

	int childNum = allChildren.Num();

	for (int i = 0; i < childNum; ++i)
	{
		allChildren[i]->RemoveFromParent();
	}
}

void UBindingOptions::CreateBindingsForMappingContext(UInputMappingContext* BindingMappingContext)
{
	const TArray<FEnhancedActionKeyMapping>& allKeyMappings = BindingMappingContext->GetMappings();

	int keyMappingsNum = allKeyMappings.Num();

	for (int i = 0; i < keyMappingsNum; ++i)
	{
		auto keyMapping = allKeyMappings[i];
		if (!keyMapping.GetPlayerMappableKeySettings() || keyMapping.Key.IsGamepadKey() != bBindGamepad)
			continue;

		UBindingOptionsItem* item = WidgetTree->ConstructWidget<UBindingOptionsItem>(BOItemClass);

		if (!item)
			continue;

		item->SetMappableKey(keyMapping);
		item->SetMappingContext(BindingMappingContext);

		item->SetIsBindingGamepad(bBindGamepad);

		FString category = keyMapping.GetPlayerMappableKeySettings()->DisplayCategory.ToString();

		BindingOptionsItems.Add(item);

		FString mappingContextName = BindingMappingContext->GetFName().ToString();
		if (mappingContextName.Contains("Controller"))
		{
			GlobalContainer->AddChild(item);
			continue;
		}

		if (mappingContextName.Contains("Inventory") || category.Contains("Inventory"))
		{
			InventoryContainer->AddChild(item);
			continue;
		}

		if (mappingContextName.Contains("PostMortem"))
		{
			SpectatorContainer->AddChild(item);
			continue;
		}

		if (category.Contains("Movement"))
			CharacterMovementContainer->AddChild(item);
		else
			CharacterGeneralContainer->AddChild(item);
	}
}

void UBindingOptions::ResetMappings()
{
	int itemsNum = BindingOptionsItems.Num();

	for (int i = 0; i < itemsNum; ++i)
	{
		auto currentItem = BindingOptionsItems[i];
		if (!IsValid(currentItem))
			continue;

		currentItem->ResetMapping();
	}
}
