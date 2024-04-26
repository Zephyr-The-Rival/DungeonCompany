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
	UFUNCTION(BlueprintCallable, Category="Voice")
	static void SetMicInputGain(float Value);

	UFUNCTION(BlueprintCallable, Category="Voice")
	static void SetMicNoiseGateTreshold(float Value); 
	
	UFUNCTION(BlueprintCallable, Category = "Voice")
	static void SetMicNoiseAttackTime(float Value);

	UFUNCTION(BlueprintCallable, Category = "Voice")
	static void SetMicNoiseReleaseTime(float Value);

	UFUNCTION(BlueprintCallable, Category = "Voice")
	static void SetMicStereoBias(float Value);

	UFUNCTION(BlueprintCallable, Category="Voice")
	static void SetVoiceSilenceDetectionTreshold(float Value);

	UFUNCTION(BlueprintCallable, Category="Voice")
	static void SetVoiceSilenceDetectionAttackTime(float Value);
	
	UFUNCTION(BlueprintCallable, Category="Voice")
	static void SetVoiceSilenceDetectionReleaseTime(float Value);

	UFUNCTION(BlueprintCallable, Category="Voice|Playback")
	static void SetVoicePlaybackShouldResync(bool Value);

	UFUNCTION(BlueprintCallable, Category="Voice|Playback")
	static void SetVoicePlaybackResyncTreshold(float Value);

	UFUNCTION(BlueprintCallable, Category="Voice")
	static void SetVoiceJitterBufferDelay(float Value);

	UFUNCTION(BlueprintCallable, Category="Voice")
	static void SetVoiceDefaultPatchGain(float Value);

	UFUNCTION(BlueprintCallable, Category="Voice")
	static void SetVoiceDefaultPatchBufferSize(float Value);

	UFUNCTION(BlueprintCallable, Category="Voice")
	static void SetVoiceDebugPrintAmplitude(bool Value);

};
