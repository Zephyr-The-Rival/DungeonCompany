// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VoiceClassManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UVoiceClassManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	TArray<USoundClass*> AvailableVoiceClasses;
	TMap<APlayerState*, USoundClass*> VoiceClassMap;

public:
	USoundClass* GetVoiceClassForPlayer(APlayerState* PlayerState);
	
	void RegisterPlayerState(APlayerState* PlayerState);
	void UnregisterPlayerState(const APlayerState* PlayerState);

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
