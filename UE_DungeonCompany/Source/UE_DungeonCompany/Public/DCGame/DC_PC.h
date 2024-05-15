// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DC_PC.generated.h"

/**
 * 
 */
 class UPlayerHud;

 class UVOIPTalker;
 class UInputAction;

UCLASS()
class UE_DUNGEONCOMPANY_API ADC_PC : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerHud> PlayerHudClass;

	UPlayerHud* MyPlayerHud;

public:
	ADC_PC();

	UPlayerHud* GetMyPlayerHud() const { return MyPlayerHud; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* PushToTalkAction;

	UPROPERTY(EditAnywhere,BlueprintGetter=IsPushToTalkActive, BlueprintSetter=SetPushToTalkActive, Category = "Microphone")
	bool bPushToTalkActive = false;

public:
	virtual void SetupInputComponent() override;

protected:
	void PushToTalkStarted();
	void PushToTalkCompleted();

public:
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	bool IsPushToTalkActive() const;

	UFUNCTION(BlueprintCallable)
	void SetPushToTalkActive(bool IsActive);


};
