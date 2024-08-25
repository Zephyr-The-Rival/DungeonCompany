// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/VoiceConfig.h"
#include "DC_VOIPTalker.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UDC_VOIPTalker : public UVOIPTalker
{
	GENERATED_BODY()

private:
	UAudioComponent* TalkerAudioComponent;

public:
	virtual void OnTalkingBegin(UAudioComponent* AudioComponent) override;
};
