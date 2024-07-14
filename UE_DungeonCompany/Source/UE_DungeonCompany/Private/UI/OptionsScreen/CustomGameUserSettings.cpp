// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OptionsScreen/CustomGameUserSettings.h"

UCustomGameUserSettings::UCustomGameUserSettings(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	gammaValue = 2.2F;
	masterVolume = 1.0F;
	musicVolume = 1.0F;
	effectsVolume = 1.0F;
	ambienceVolume = 1.0F;
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
