// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiDevice_PC.generated.h"

/**
 * 
 */

class UInputAction;
class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;

UCLASS()
class UE_DUNGEONCOMPANY_API AMultiDevice_PC : public APlayerController
{
	GENERATED_BODY()

private:
	bool bUsingGamepad = false;
	
	TArray<UInputMappingContext*> AllMappingContexts;

public:
	inline const TArray<UInputMappingContext*>& GetAllMappingContexts() const { return AllMappingContexts; }

public:
	AMultiDevice_PC();

protected:
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline bool IsUsingGamepad() const { return bUsingGamepad; }

public:
	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputDeviceChanged, bool, IsUsingGamepad);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInputDeviceChanged OnInputDeviceChanged;
	
public:
	virtual void SetupInputComponent() override;

protected:
	void OnAnyKeyPressed(const FKey& Key);

private:
	mutable TMap<int64, FKey> ActionKeyCache;

public:
	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKeyMappingChanged);
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnKeyMappingChanged OnKeyMappingChanged;
	
public:
	UEnhancedInputLocalPlayerSubsystem* GetInputLocalPlayer() const;
	void UpdateMapping(FName MappingName, FKey NewKey) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FKey GetCurrentKeyForMapping(FName MappingName) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FKey GetCurrentKeyForAction(UInputAction* InputAction) const;
};
