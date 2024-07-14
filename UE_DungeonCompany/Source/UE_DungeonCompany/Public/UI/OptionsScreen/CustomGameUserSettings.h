 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "CustomGameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UCustomGameUserSettings : public UGameUserSettings
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(Config)
	float gammaValue;

	UPROPERTY(Config)
	float masterVolume;

	UPROPERTY(Config)
	float musicVolume;

	UPROPERTY(Config)
	float effectsVolume;

	UPROPERTY(Config)
	float ambienceVolume;

	UPROPERTY(Config)
	bool bIsToggleCrouch;
	
	UPROPERTY(Config)
	bool bIsToggleSprint;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Audio")
	TSoftObjectPtr<USoundMix> MasterSoundMix;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Audio")
	TSoftObjectPtr<USoundClass> SC_Master;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Audio")
	TSoftObjectPtr<USoundClass> SC_Music;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Audio")
	TSoftObjectPtr<USoundClass> SC_Effects;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Audio")
	TSoftObjectPtr<USoundClass> SC_Ambience;

public:

	UFUNCTION(BlueprintCallable)
	static UCustomGameUserSettings* GetCustomGameUserSettings();

	UFUNCTION(BlueprintCallable)
	void SetGammaValue(float newGammaValue);

	UFUNCTION(BlueprintPure)
	float GetGammaValue() const;

	UFUNCTION(BlueprintCallable)
	void SetMasterVolume(float newValue);

	UFUNCTION(BlueprintPure)
	float GetMasterVolume() const;
		
	UFUNCTION(BlueprintCallable)
	void SetMusicVolume(float newValue);

	UFUNCTION(BlueprintPure)
	float GetMusicVolume() const;
		
	UFUNCTION(BlueprintCallable)
	void SetEffectsVolume(float newValue);

	UFUNCTION(BlueprintPure)
	float GetEffectsVolume() const;

	UFUNCTION(BlueprintCallable)
	void SetAmbianceVolume(float newValue);

	UFUNCTION(BlueprintPure)
	float GetAmbianceVolume() const;

	UFUNCTION(BlueprintCallable)
	void SetToggleCrouch(bool bNewToggleCrouch);

	UFUNCTION(BlueprintPure)
	bool GetToggleCrouch() const;

	UFUNCTION(BlueprintCallable)
	void SetToggleSprint(bool bNewToggleSprint);

	UFUNCTION(BlueprintPure)
	bool GetToggleSprint() const;

	virtual void ApplySettings(bool bCheckForCommandLineOverride) override;

	UFUNCTION(BlueprintCallable)
	void ApplyAudioSettings();
};
