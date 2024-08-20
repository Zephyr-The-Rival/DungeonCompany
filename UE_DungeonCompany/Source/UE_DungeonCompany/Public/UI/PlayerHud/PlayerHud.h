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
class ABuyableItem;

UENUM(BlueprintType)
enum EDirections
{
	Up UMETA(DisplayName="Up"),
	Down UMETA(DisplayName = "Down"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
};

class UInventorySlot;

USTRUCT(BlueprintType)
struct FSlotData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsBackpackSlot=false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInventorySlot* Slot;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSwichDoneDelegate);

class APlayerCharacter;

UCLASS()
class UE_DUNGEONCOMPANY_API UPlayerHud : public UUserWidget
{
	GENERATED_BODY()
	
public:



	UFUNCTION(BlueprintNativeEvent)
	void ShowCrosshair(UTexture2D* newTexture);

	virtual void ShowCrosshair_Implementation(UTexture2D* newTexture);

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
	void ShowTextInteractPrompt(const FString& NewText);
	void ShowTextInteractPrompt_Implementation(const FString& NewText);

	UFUNCTION(BlueprintNativeEvent)
	void ShowBuyPrompt(ABuyableItem* BuyableItem);
	void ShowBuyPrompt_Implementation(ABuyableItem* BuyableItem);

	UFUNCTION(BlueprintNativeEvent)
	void UpdateBuffs();
	void UpdateBuffs_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void ShowInteractProgressBar(float MaxTime);
	void ShowInteractProgressBar_Implementation(float MaxTime);

	UFUNCTION(BlueprintNativeEvent)
	void HideInteractProgressBar();
	void HideInteractProgressBar_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	UUserWidget* ShowSelectionWheel(const TArray<FString>& Options);
	UUserWidget* ShowSelectionWheel_Implementation( const TArray<FString>& Options);

	UFUNCTION(BlueprintNativeEvent)
	int DestroySelectionWheel();
	int DestroySelectionWheel_Implementation();


};
