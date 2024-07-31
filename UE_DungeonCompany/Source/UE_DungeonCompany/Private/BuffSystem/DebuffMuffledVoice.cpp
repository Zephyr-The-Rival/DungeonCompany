// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffSystem/DebuffMuffledVoice.h"
#include "PlayerCharacter/PlayerCharacter.h"

UDebuffMuffledVoice::UDebuffMuffledVoice()
{
	static ConstructorHelpers::FObjectFinder<USoundEffectSourcePresetChain> muffledSoundEffect(TEXT("/Game/_DungeonCompanyContent/Audio/Player/MuffledVoiceEC.MuffledVoiceEC"));
	MuffledSoundEffect = muffledSoundEffect.Object;
}

void UDebuffMuffledVoice::Apply()
{
	Super::Apply();

	APlayerCharacter* character = GetOuterEntity<APlayerCharacter>();

	if(!character)
		return;

	character->SetVoiceEffect(MuffledSoundEffect);
}

void UDebuffMuffledVoice::Remove()
{
	APlayerCharacter* character = GetOuterEntity<APlayerCharacter>();

	if (!character)
		return;

	character->SetVoiceEffect(nullptr);
}
