// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PlayerOptions.generated.h"

class UCommonButtonBase;
class UCommonValueChanger;
class USoundMix;

UCLASS()
class UE_DUNGEONCOMPANY_API UPlayerOptions : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta =(BindWidget))
	UCommonButtonBase* KickButton;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta = (BindWidget))
	UCommonValueChanger* VolumeSlider;
	
	UPROPERTY(BlueprintReadOnly)
	APlayerState* ControllingPlayer;
	
private:
	USoundMix* MasterSoundMix;

public:
	void SetControllingPlayer(APlayerState* PlayerState);

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
