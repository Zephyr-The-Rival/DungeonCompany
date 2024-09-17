// Copyright Epic Games, Inc. All Rights Reserved.

#include "VoiceEngineSteamHmT.h"
#include "OnlineSubsystemSteamPrivate.h" // IWYU pragma: keep
#include "VoiceModule.h"

FVoiceEngineSteamHmT::FVoiceEngineSteamHmT(IOnlineSubsystem* InSubsystem) :
	FVoiceEngineImpl(InSubsystem),
	SteamUserPtr(SteamUser()),
	SteamFriendsPtr(SteamFriends()),
	CaptureDeviceName("")
{
}

FVoiceEngineSteamHmT::~FVoiceEngineSteamHmT()
{
	if (IsRecording())
	{
		SteamFriendsPtr->SetInGameVoiceSpeaking(SteamUserPtr->GetSteamID(), false);
	}
}

void FVoiceEngineSteamHmT::UseCaptureDevice(const FString& DeviceName)
{
	if (!GetVoiceCapture().IsValid())
	{
		GetVoiceCapture() = FVoiceModule::Get().CreateVoiceCapture(DeviceName);
		CaptureDeviceName = DeviceName;

		return;
	}

	if (GetVoiceCapture()->ChangeDevice(DeviceName, UVOIPStatics::GetVoiceSampleRate(),
	                                    UVOIPStatics::GetVoiceNumChannels()))
	{
		CaptureDeviceName = DeviceName;
	}
}

void FVoiceEngineSteamHmT::StartRecording() const
{
	UE_LOG_ONLINE_VOICEENGINE(VeryVerbose, TEXT("VOIP StartRecording"));
	if (GetVoiceCapture().IsValid())
	{
		if (!GetVoiceCapture()->Start())
		{
			UE_LOG_ONLINE_VOICEENGINE(Warning, TEXT("Failed to start voice recording"));
		}
		else if (SteamFriendsPtr)
		{
			SteamFriendsPtr->SetInGameVoiceSpeaking(SteamUserPtr->GetSteamID(), true);
		}
	}
}

void FVoiceEngineSteamHmT::StoppedRecording() const
{
	UE_LOG_ONLINE_VOICEENGINE(VeryVerbose, TEXT("VOIP StoppedRecording"));
	if (SteamFriendsPtr)
	{
		SteamFriendsPtr->SetInGameVoiceSpeaking(SteamUserPtr->GetSteamID(), false);
	}
}
