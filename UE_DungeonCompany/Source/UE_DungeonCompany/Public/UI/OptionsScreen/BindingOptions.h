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
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBindingOptionsItem> BOItemClass;

	UPROPERTY(EditAnywhere)
	bool bBindGamepad = false;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* CharacterGeneralContainer;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* CharacterMovementContainer;
	
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* InventoryContainer;
	
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* SpectatorContainer;
	
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* GlobalContainer;

protected:
	virtual void NativeConstruct() override;

private:
	TArray<UBindingOptionsItem*> BindingOptionsItems;

protected:
	void CreateBindingsForMappingContext(UInputMappingContext* BindingMappingContext);
	
public:
	UFUNCTION(BlueprintCallable)
	void ResetMappings();
	
};
