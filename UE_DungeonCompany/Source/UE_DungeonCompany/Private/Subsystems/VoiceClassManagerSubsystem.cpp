// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/VoiceClassManagerSubsystem.h"

#include "Engine/ObjectLibrary.h"
#include "GameFramework/PlayerState.h"
#include "HAL/FileManagerGeneric.h"
#include "Sound/SoundClass.h"

USoundClass* UVoiceClassManagerSubsystem::GetVoiceClassForPlayer(APlayerState* PlayerState)
{
	if (!VoiceClassMap.Contains(PlayerState))
	{
		RegisterPlayerState(PlayerState);
	}

	return VoiceClassMap[PlayerState];
}

void UVoiceClassManagerSubsystem::RegisterPlayerState(APlayerState* PlayerState)
{
	if (!IsValid(PlayerState) || AvailableVoiceClasses.IsEmpty() || VoiceClassMap.Contains(PlayerState))
		return;

	VoiceClassMap.Add(PlayerState, AvailableVoiceClasses.Pop());
}

void UVoiceClassManagerSubsystem::UnregisterPlayerState(const APlayerState* PlayerState)
{
	if (!VoiceClassMap.Contains(PlayerState))
		return;

	AvailableVoiceClasses.Push(VoiceClassMap[PlayerState]);
	VoiceClassMap.Remove(PlayerState);
}

void UVoiceClassManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
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