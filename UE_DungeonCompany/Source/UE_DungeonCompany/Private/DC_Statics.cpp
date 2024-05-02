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

void UDC_Statics::SetMicNoiseAttackTime(float Value)
{
	static IConsoleVariable* micNoiseAttackTime = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.MicNoiseAttackTime"));
	micNoiseAttackTime->Set(Value);
}

void UDC_Statics::SetMicNoiseReleaseTime(float Value)
{
	static IConsoleVariable* micNoiseReleaseTime = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.MicNoiseReleaseTime"));
	micNoiseReleaseTime->Set(Value);
}

void UDC_Statics::SetMicStereoBias(float Value)
{
	static IConsoleVariable* micStereoBias = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.MicStereoBias"));
	micStereoBias->Set(Value);
}

void UDC_Statics::SetVoiceSilenceDetectionTreshold(float Value)
{
	static IConsoleVariable* silenceDetectionThreshold = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.SilenceDetectionThreshold"));
	silenceDetectionThreshold->Set(Value);
}

void UDC_Statics::SetVoiceSilenceDetectionAttackTime(float Value)
{
	static IConsoleVariable* silenceDetectionAttackTime = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.SilenceDetectionAttackTime"));
	silenceDetectionAttackTime->Set(Value);
}

void UDC_Statics::SetVoiceSilenceDetectionReleaseTime(float Value)
{
	static IConsoleVariable* silenceDetectionReleaseTime = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.SilenceDetectionReleaseTime"));
	silenceDetectionReleaseTime->Set(Value);
}

void UDC_Statics::SetVoicePlaybackShouldResync(bool Value)
{
	static IConsoleVariable* shouldResync = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.playback.ShouldResync"));
	shouldResync->Set(Value);
}

void UDC_Statics::SetVoicePlaybackResyncTreshold(float Value)
{
	static IConsoleVariable* resyncThreshold = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.playback.ResyncThreshold"));
	resyncThreshold->Set(Value);
}

void UDC_Statics::SetVoiceJitterBufferDelay(float Value)
{
	static IConsoleVariable* jitterBufferDelay = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.JitterBufferDelay"));
	jitterBufferDelay->Set(Value);
}

void UDC_Statics::SetVoiceDefaultPatchGain(float Value)
{
	static IConsoleVariable* defaultPatchGain = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.DefaultPatchGain"));
	defaultPatchGain->Set(Value);
}

void UDC_Statics::SetVoiceDefaultPatchBufferSize(float Value)
{
	static IConsoleVariable* defaultPatchBufferSize = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.DefaultPatchBufferSize"));
	defaultPatchBufferSize->Set(Value);
}

void UDC_Statics::SetVoiceDebugPrintAmplitude(bool Value)
{
	static IConsoleVariable* printAmplitude = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.debug.PrintAmplitude"));
	printAmplitude->Set(Value);
}
