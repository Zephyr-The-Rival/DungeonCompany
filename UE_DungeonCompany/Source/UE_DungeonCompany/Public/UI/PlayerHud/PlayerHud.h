// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHud.generated.h"

/**
 * 
 */

class APlayerCharacter;
class UInventorySlot;
class UInputAction;

UENUM(BlueprintType)
enum EDirections
{
	Up UMETA(DisplayName="Up"),
	Down UMETA(DisplayName = "Down"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
};

USTRUCT(BlueprintType)
struct FSlotData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	bool bIsBackpackSlot;
	UPROPERTY(BlueprintReadWrite)
	UInventorySlot* Slot;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSwichDoneDelegate);



UCLASS()
class UE_DUNGEONCOMPANY_API UPlayerHud : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	APlayerCharacter* MyCharacter;

	UFUNCTION(BlueprintNativeEvent)
	void ShowCrosshair(const FString& Text, UInputAction* InputAction);

	virtual void ShowCrosshair_Implementation(const FString& Text, UInputAction* InputAction);

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
	FSlotData GetHighlightedSlot();


	UFUNCTION(BlueprintNativeEvent)
	void MoveHighlight(EDirections direction);
	void MoveHighlight_Implementation(EDirections direction);

	
	UFUNCTION(BlueprintNativeEvent)
	void UpdateCrouchIcon();
	void UpdateCrouchIcon_Implementation();


	UFUNCTION(BlueprintNativeEvent)
	void ToggleOptionsMenu(bool On);
	void ToggleOptionsMenu_Implementation(bool On);
};
