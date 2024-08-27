// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/VoiceChatSubsystem.h"

#include "EngineUtils.h"
#include "OnlineSubsystem.h"
#include "Engine/ObjectLibrary.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/VoiceInterface.h"
#include "Sound/SoundClass.h"
#include "OnlineSubsystemUtils.h"
#include "Components/AudioComponent.h"

void UVoiceChatSubsystem::RegisterAudioComponentForPlayerState(APlayerState* PlayerState,
	UAudioComponent* AudioComponent)
{
	if(!IsValid(AudioComponent))
		return;
	
	bool bAlreadyRegistered = AudioComponentMap.Contains(PlayerState);
	if(bAlreadyRegistered && AudioComponentMap[PlayerState] == AudioComponent)
		return;

	if(bAlreadyRegistered)
		AudioComponentMap.Remove(PlayerState);

	AudioComponentMap.Add(PlayerState, AudioComponent);
	AudioComponent->SetVolumeMultiplier(GetVolumeForPlayerState(PlayerState));
}

float UVoiceChatSubsystem::GetVolumeForPlayerState(APlayerState* PlayerState)
{
	if(!VolumeMap.Contains(PlayerState))
		VolumeMap.Add(PlayerState, 1.0f);

	return VolumeMap[PlayerState];		
}

void UVoiceChatSubsystem::SetVolumeForPlayerState(APlayerState* PlayerState, float Volume)
{
	VolumeMap.FindOrAdd(PlayerState) = Volume;
	
	if(AudioComponentMap.Contains(PlayerState))
		AudioComponentMap[PlayerState]->SetVolumeMultiplier(Volume);
}

void UVoiceChatSubsystem::MutePlayer(const APlayerState* PlayerState) const
{
	if (!IsValid(PlayerState))
		return;

	Online::GetVoiceInterface(GetWorld())->MuteRemoteTalker(0, *PlayerState->GetUniqueId(), false);
}

void UVoiceChatSubsystem::UnmutePlayer(const APlayerState* PlayerState) const
{
	if (!IsValid(PlayerState))
		return;

	Online::GetVoiceInterface(GetWorld())->UnmuteRemoteTalker(0, *PlayerState->GetUniqueId(), false);
}

void UVoiceChatSubsystem::UnmuteAllPlayers() const
{
	for (TActorIterator<APlayerState> It(GetWorld()); It; ++It)
		UnmutePlayer(*It);
}
