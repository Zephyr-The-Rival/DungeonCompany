// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/VoiceChatSubsystem.h"

#include "EngineUtils.h"
#include "OnlineSubsystem.h"
#include "Engine/ObjectLibrary.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/VoiceInterface.h"
#include "Sound/SoundClass.h"
#include "OnlineSubsystemUtils.h"

USoundClass* UVoiceChatSubsystem::GetVoiceClassForPlayer(APlayerState* PlayerState)
{
	if (!VoiceClassMap.Contains(PlayerState))
	{
		RegisterPlayerState(PlayerState);
	}

	return VoiceClassMap.Contains(PlayerState)? VoiceClassMap[PlayerState] : nullptr;
}

void UVoiceChatSubsystem::RegisterPlayerState(APlayerState* PlayerState)
{
	if (!IsValid(PlayerState) || AvailableVoiceClasses.IsEmpty() || VoiceClassMap.Contains(PlayerState))
		return;

	VoiceClassMap.Add(PlayerState, AvailableVoiceClasses.Pop());
}

void UVoiceChatSubsystem::UnregisterPlayerState(const APlayerState* PlayerState)
{
	if (!VoiceClassMap.Contains(PlayerState))
		return;

	AvailableVoiceClasses.Push(VoiceClassMap[PlayerState]);
	VoiceClassMap.Remove(PlayerState);
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

void UVoiceChatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UObjectLibrary* objectLibrary = UObjectLibrary::CreateLibrary(USoundClass::StaticClass(), false, GIsEditor);

	if (!objectLibrary)
		return;

	objectLibrary->AddToRoot();

	FString path = TEXT("/Game/_DungeonCompanyContent/Audio/VoiceClasses/");
	int voiceClassesNum = objectLibrary->LoadAssetsFromPath(path);

	objectLibrary->GetObjects<USoundClass>(AvailableVoiceClasses);
}
