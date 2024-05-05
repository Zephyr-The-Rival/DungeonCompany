// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHud.generated.h"

/**
 * 
 */

class APlayerCharacter;

UCLASS()
class UE_DUNGEONCOMPANY_API UPlayerHud : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	APlayerCharacter* MyCharacter;

	UFUNCTION(BlueprintNativeEvent)
	void ShowCrosshair(const FString& Text);

	virtual void ShowCrosshair_Implementation(const FString& Text);

	UFUNCTION(BlueprintNativeEvent)
	void HideCrosshair();

	virtual void HideCrosshair_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void FocusOnInventorySlot(int32 Index);

	virtual void FocusOnInventorySlot_Implementation(int32 Index);

};
