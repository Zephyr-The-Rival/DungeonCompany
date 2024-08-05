// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "Blueprint/UserWidget.h"
#include "BindingOptionsItem.generated.h"

/**
 * 
 */

class UInputMappingContext;
class UTextBlock;
class UInputKeySelector;
class UPlayerMappableKeySettings;

UCLASS()
class UE_DUNGEONCOMPANY_API UBindingOptionsItem : public UUserWidget
{
	GENERATED_BODY()

private:
	FEnhancedActionKeyMapping MappableKey;
	UInputMappingContext* MappingContext;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* KeyText;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UInputKeySelector* KeySelector;

public:
	void SetMappableKey(const FEnhancedActionKeyMapping& InMappableKey);
	void SetMappingContext(UInputMappingContext* InMappingContext);
	
protected:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void OnKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void OnIsSelectingKeyChanged();
	

	
	
	
};
