// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DC_PC.generated.h"

/**
 * 
 */
 class UPlayerHud;

 class UVOIPTalker;
 class UInputMappingContext;
 class UInputAction;

UCLASS()
class UE_DUNGEONCOMPANY_API ADC_PC : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerHud> PlayerHudClass;

	UPROPERTY(BlueprintGetter = GetMyPlayerHud)
	UPlayerHud* MyPlayerHud;

public:
	ADC_PC();

	UFUNCTION(BlueprintCallable)
	UPlayerHud* GetMyPlayerHud() const { return MyPlayerHud; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input | Mapping")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* PushToTalkAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* OptionsAction;

	UPROPERTY(EditAnywhere,BlueprintGetter=IsPushToTalkActive, BlueprintSetter=SetPushToTalkActive, Category = "Microphone")
	bool bPushToTalkActive = false;

	bool bUsingGamepad = false;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline bool IsUsingGamepad() const { return bUsingGamepad; }

	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputDeviceChanged, bool, IsUsingGamepad);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInputDeviceChanged OnInputDeviceChanged;

public:
	virtual void SetupInputComponent() override;

private:
	bool bOptionsMenuIsOn = false;

protected:
	void ToggleOptions();
	
	void PushToTalkStarted();
	void PushToTalkCompleted();

	void OnAnyKeyPressed(const FKey& Key);

public:
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	bool IsPushToTalkActive() const;

	UFUNCTION(BlueprintCallable)
	void SetPushToTalkActive(bool IsActive);

};
