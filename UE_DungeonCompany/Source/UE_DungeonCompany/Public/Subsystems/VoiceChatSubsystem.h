// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VoiceChatSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UVoiceChatSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	TMap<APlayerState*, UAudioComponent*> AudioComponentMap;
	TMap<APlayerState*, float> VolumeMap;

public:
	void RegisterAudioComponentForPlayerState(APlayerState* PlayerState, UAudioComponent* AudioComponent);

	float GetVolumeForPlayerState(APlayerState* PlayerState);
	void SetVolumeForPlayerState(APlayerState* PlayerState, float Volume);

	void MutePlayer(const APlayerState* PlayerState) const;
	void UnmutePlayer(const APlayerState* PlayerState) const;
	
	void UnmuteAllPlayers() const;
};
