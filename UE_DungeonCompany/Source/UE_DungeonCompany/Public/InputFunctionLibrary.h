// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InputFunctionLibrary.generated.h"

/**
 * 
 */
struct FInputActionValue;

UCLASS()
class UE_DUNGEONCOMPANY_API UInputFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void LookMouse(const FVector2d& Value, APawn* Player);
	static void LookGamepad(const FVector2d& Value, APawn* Player);
	
};
