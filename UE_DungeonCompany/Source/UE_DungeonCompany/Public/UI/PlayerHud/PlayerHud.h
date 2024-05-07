// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHud.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum EDirections
{
	Up UMETA(DisplayName="Up"),
	Down UMETA(DisplayName = "Down"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSwichDoneDelegate);

class APlayerCharacter;
class UInventorySlot;

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
	void ToggleInventory(bool On);
	void ToggleInventory_Implementation(bool On);

	UFUNCTION(BlueprintNativeEvent)
	void RefreshInventory();
	void RefreshInventory_Implementation();


	UFUNCTION(BlueprintNativeEvent)
	void SwichHandDisplays(bool ToHandA);
	void SwichHandDisplays_Implementation(bool ToHandA);

	UPROPERTY(BlueprintCallable, Category = "Events")
    FSwichDoneDelegate OnSwichingDone;

	UFUNCTION(BlueprintImplementableEvent)
	UInventorySlot* GetHighlightedSlot();


	UFUNCTION(BlueprintNativeEvent)
	void MoveHighlight(EDirections direction);
	void MoveHighlight_Implementation(EDirections direction);


};
