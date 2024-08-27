// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OptionsScreen/CustomGameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

UCustomGameUserSettings::UCustomGameUserSettings(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	gammaValue = 2.2F;
	masterVolume = 1.0F;
	musicVolume = 1.0F;
	effectsVolume = 1.0F;
	ambienceVolume = 1.0F;
	bIsToggleCrouch = false;
	bIsToggleSprint = false;
}

UCustomGameUserSettings* UCustomGameUserSettings::GetCustomGameUserSettings()
{
	return Cast<UCustomGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void UCustomGameUserSettings::SetGammaValue(float newGammaValue)
{
	gammaValue = newGammaValue;
}

float UCustomGameUserSettings::GetGammaValue() const
{
	return gammaValue;
}

void UCustomGameUserSettings::SetMasterVolume(float newValue)
{
	masterVolume = newValue;
}

float UCustomGameUserSettings::GetMasterVolume() const
{
	return masterVolume;
}

void UCustomGameUserSettings::SetMusicVolume(float newValue)
{
	musicVolume = newValue;
}

float UCustomGameUserSettings::GetMusicVolume() const
{
	return musicVolume;
}

void UCustomGameUserSettings::SetEffectsVolume(float newValue)
{
	effectsVolume = newValue;
}

float UCustomGameUserSettings::GetEffectsVolume() const
{
	return effectsVolume;
}

void UCustomGameUserSettings::SetAmbianceVolume(float newValue)
{
	ambienceVolume = newValue;
}

float UCustomGameUserSettings::GetAmbianceVolume() const
{
	return ambienceVolume;
}

void UCustomGameUserSettings::SetToggleCrouch(bool bNewToggleCrouch)
{
	bIsToggleCrouch = bNewToggleCrouch;
}

bool UCustomGameUserSettings::GetToggleCrouch() const
{
	return bIsToggleCrouch;
}

void UCustomGameUserSettings::SetToggleSprint(bool bNewToggleSprint)
{
	bIsToggleSprint = bNewToggleSprint;
}

bool UCustomGameUserSettings::GetToggleSprint() const
{
	return bIsToggleSprint;
}

void UCustomGameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);

	FString GammaCommand = FString::Printf(TEXT("gamma %f"), gammaValue);
	GEngine->Exec(GetWorld(), *GammaCommand);

	if (MasterSoundMix.IsValid())
	{
		if (SC_Master.IsValid())
		{
			UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix.Get(), SC_Master.Get(), masterVolume, 1.0f, 0.1f, true);
		}
		if (SC_Music.IsValid())
		{
			UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix.Get(), SC_Music.Get(), musicVolume, 1.0f, 0.1f, true);
		}
		if (SC_Effects.IsValid())
		{
			UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix.Get(), SC_Effects.Get(), effectsVolume, 1.0f, 0.1f, true);
		}
		if (SC_Ambience.IsValid())
		{
			UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix.Get(), SC_Ambience.Get(), ambienceVolume, 1.0f, 0.1f, true);
		}
		UGameplayStatics::PushSoundMixModifier(this, MasterSoundMix.Get());
	}
}

void UCustomGameUserSettings::ApplyAudioSettings()
{
	if (SC_Master.IsValid())
	{
		UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix.Get(), SC_Master.Get(), masterVolume, 1.0f, 0.1f, true);
	}
	if (SC_Music.IsValid())
	{
		UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix.Get(), SC_Music.Get(), musicVolume, 1.0f, 0.1f, true);
	}
	if (SC_Effects.IsValid())
	{
		UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix.Get(), SC_Effects.Get(), effectsVolume, 1.0f, 0.1f, true);
	}
	if (SC_Ambience.IsValid())
	{
		UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix.Get(), SC_Ambience.Get(), ambienceVolume, 1.0f, 0.1f, true);
	}
	UGameplayStatics::PushSoundMixModifier(this, MasterSoundMix.Get());
}

void UCustomGameUserSettings::ApplyGammaSettings()
{
	FString GammaCommand = FString::Printf(TEXT("gamma %f"), gammaValue);
	GEngine->Exec(GetWorld(), *GammaCommand);
}
