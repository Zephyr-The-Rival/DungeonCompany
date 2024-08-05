// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BindingOptions.generated.h"

class UInputMappingContext;
class UVerticalBox;
class UBindingOptionsItem;
/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UBindingOptions : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Mappings")
	TArray<UInputMappingContext*> AllMappings;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBindingOptionsItem> BOItemClass;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* ControllerContainer;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* MKContainer;

protected:
	virtual void NativeConstruct() override;

protected:
	void CreateBindingsForMappingContext(UInputMappingContext* BindingMappingContext);
	
};
