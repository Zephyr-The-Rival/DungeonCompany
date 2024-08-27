// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonValueChanger.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UCommonValueChanger : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	void OnValueChanged(float NewValue);
	
public:
	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueChanged, float, NewValue);

	FOnValueChanged OnValueChangedEvent;
};
