// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PlayerOptions.generated.h"

class UCommonValueChanger;
class USoundMix;

UCLASS()
class UE_DUNGEONCOMPANY_API UPlayerOptions : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (BindWidget))
	UCommonValueChanger* VolumeSlider;

private:
	APlayerState* ControllingPlayer;
	USoundMix* MasterSoundMix;

public:
	UPlayerOptions();

protected:
	virtual void NativeConstruct() override;

	bool IsControllingPlayerValid();

protected:
	UFUNCTION()
	void OnVolumeSliderValueChanged(float NewValue);

public:
	UFUNCTION(BlueprintCallable)
	void KickPlayer();
};
