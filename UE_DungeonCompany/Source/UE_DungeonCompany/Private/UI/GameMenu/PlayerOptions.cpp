// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameMenu/PlayerOptions.h"

#include "UI/Common/CommonValueChanger.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/VoiceChatSubsystem.h"
#include "Sound/SoundMix.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerState.h"

UPlayerOptions::UPlayerOptions()
{
	static ConstructorHelpers::FObjectFinder<USoundMix> MasterSoundMixFinder(
		TEXT("/Script/Engine.SoundMix'/Game/_DungeonCompanyContent/Audio/MasterSoundMix.MasterSoundMix'"));
	MasterSoundMix = MasterSoundMixFinder.Object;
}

void UPlayerOptions::NativeConstruct()
{
	Super::NativeConstruct();

	VolumeSlider->OnValueChangedEvent.AddDynamic(this, &UPlayerOptions::OnVolumeSliderValueChanged);
}

bool UPlayerOptions::IsControllingPlayerValid()
{
	if(IsValid(ControllingPlayer))
		return true;

	RemoveFromParent();
	return false;
}

void UPlayerOptions::OnVolumeSliderValueChanged(float NewValue)
{
	if(!IsControllingPlayerValid())
		return;
	
	USoundClass* voiceClass = GetGameInstance()->GetSubsystem<UVoiceChatSubsystem>()->GetVoiceClassForPlayer(
		ControllingPlayer);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, voiceClass, NewValue * 0.01);
}

void UPlayerOptions::KickPlayer()
{
	if (!GetOwningPlayer()->HasAuthority() || !IsControllingPlayerValid())
		return;

	GetWorld()->GetAuthGameMode()->GameSession->KickPlayer(ControllingPlayer->GetPlayerController(),
														   FText::FromString("You're not welcome here!!! >:("));
}