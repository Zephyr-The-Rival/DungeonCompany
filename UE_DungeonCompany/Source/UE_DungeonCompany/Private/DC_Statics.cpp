// Fill out your copyright notice in the Description page of Project Settings.


#include "DC_Statics.h"

void UDC_Statics::SetMicInputGain(float Value)
{
	static IConsoleVariable* micInputGain = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.MicInputGain"));
	micInputGain->Set(Value);
}

void UDC_Statics::SetMicNoiseGateTreshold(float Value)
{
	static IConsoleVariable* micNoiseGateTreshold = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.MicNoiseGateThreshold"));
	micNoiseGateTreshold->Set(Value);
}
