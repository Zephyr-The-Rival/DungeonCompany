// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiDevice_PC.generated.h"

/**
 * 
 */

class UInputModifier;

USTRUCT(BlueprintType)
struct FCustomMapping
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey MappedKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UInputModifier*> Modifiers;
};

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

private:
	mutable TMap<UInputAction*, TTuple<TArray<FCustomMapping>, TArray<FCustomMapping>>> ActionKeyCache;
	
public:
	const TTuple<TArray<FCustomMapping>, TArray<FCustomMapping>>& GetCurrentKeysForAction(UInputAction* InputAction) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<FCustomMapping>& GetKeyboardKeysForAction(UInputAction* InputAction) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<FCustomMapping>& GetGamepadKeysForAction(UInputAction* InputAction) const;
};
