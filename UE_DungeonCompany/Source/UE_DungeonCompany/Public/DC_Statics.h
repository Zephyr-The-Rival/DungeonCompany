// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DC_Statics.generated.h"

#define LogWarning(Message) UE_LOG(LogTemp, Warning, TEXT("%s"),Message)
#define PrintMessage(Message, DisplayLength) GEngine->AddOnScreenDebugMessage(-1, DisplayLength, FColor::Green, Message)

UCLASS()
class UE_DUNGEONCOMPANY_API UDC_Statics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Microphone")
	static void SetMicInputGain(float Value);

	UFUNCTION(BlueprintCallable, Category="Microphone")
	static void SetMicNoiseGateTreshold(float Value);
	
};
