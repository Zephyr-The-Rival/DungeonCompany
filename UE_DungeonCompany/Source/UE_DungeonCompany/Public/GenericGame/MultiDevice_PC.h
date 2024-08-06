// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiDevice_PC.generated.h"

/**
 * 
 */

class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;

UCLASS()
class UE_DUNGEONCOMPANY_API AMultiDevice_PC : public APlayerController
{
	GENERATED_BODY()

private:
	bool bUsingGamepad = false;
	TArray<UInputMappingContext*> AllMappings;

public:
	inline const TArray<UInputMappingContext*>& GetAllMappingContexts() const { return AllMappings; }

public:
	AMultiDevice_PC();

protected:
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline bool IsUsingGamepad() const { return bUsingGamepad; }

	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputDeviceChanged, bool, IsUsingGamepad);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInputDeviceChanged OnInputDeviceChanged;

	virtual void SetupInputComponent() override;

protected:
	void OnAnyKeyPressed(const FKey& Key);
	
public:
	UEnhancedInputLocalPlayerSubsystem* GetInputLocalPlayer() const;
	void UpdateMapping(FName MappingName, FKey NewKey);

	FKey GetCurrentKeyForMapping(FName MappingName) const;
	
};
