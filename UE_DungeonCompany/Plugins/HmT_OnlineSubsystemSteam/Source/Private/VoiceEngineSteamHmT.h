// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VoiceEngineImpl.h"

class IOnlineSubsystem;
class FUniqueNetIdSteam;

#define INVALID_INDEX -1

/**
* Generic implementation of voice engine, using Voice module for capture/codec
*/
class FVoiceEngineSteamHmT : public FVoiceEngineImpl
{
	/** Steam User interface */
	class ISteamUser* SteamUserPtr;
	/** Steam Friends interface */
	class ISteamFriends* SteamFriendsPtr;

	/** Start capturing voice data */
	virtual void StartRecording() const override;

	/** Called when "last half second" is over */
	virtual void StoppedRecording() const override;

PACKAGE_SCOPE:
	/** Constructor */
	FVoiceEngineSteamHmT() :
		FVoiceEngineImpl(),
		SteamUserPtr(nullptr),
		SteamFriendsPtr(nullptr)
	{
	};

public:
	FVoiceEngineSteamHmT(IOnlineSubsystem* InSubsystem);
	virtual ~FVoiceEngineSteamHmT();

private:
	FString CaptureDeviceName;

public:
	virtual void UseCaptureDevice(const FString& DeviceName);
};
