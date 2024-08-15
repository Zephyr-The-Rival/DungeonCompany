// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericGame/MultiDevice_PC.h"
#include "DC_PC.generated.h"

/**
 * 
 */
 class UPlayerHud;

 class UVOIPTalker;
 class UInputMappingContext;
 class UInputAction;

UENUM(BlueprintType)
enum class EPawnType : uint8 {
     None = 0 UMETA(DisplayName = "None"),
     Gameplay = 1  UMETA(DisplayName = "Gameplay"),
     Spectator = 2     UMETA(DisplayName = "Spectator"),
};

UCLASS()
class UE_DUNGEONCOMPANY_API ADC_PC : public AMultiDevice_PC
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPawnType PawnType;

public:
	ADC_PC();

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

public:
	void SetPawnType(EPawnType NewPawnType);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnPawnTypeChanged(EPawnType NewPawnType);
	virtual void OnPawnTypeChanged_Implementation(EPawnType NewPawnType);

public:
	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnTypeChanged, EPawnType, NewPawnType);

	FOnPawnTypeChanged EventOnPawnTypeChanged;

private:
	UPROPERTY(EditAnywhere, Category="Balancing/Controls")
	float GamepadAccelerationSpeed = 7.f;

	float LastLookVectorLength = 0.f;

public:
	inline float GetGamePadAccelerationSpeed() const { return GamepadAccelerationSpeed; }
	void SetGamePadAccelerationSpeed(float InSpeed);

	inline float GetLastLookVectorLength() const { return LastLookVectorLength; }
	void SetLastLookVectorLength(float InLength);

private:
	UPROPERTY(EditAnywhere, Category = "Input | Mapping")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* PushToTalkAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* OptionsAction;

	UPROPERTY(EditAnywhere,BlueprintGetter=IsPushToTalkActive, BlueprintSetter=SetPushToTalkActive, Category = "Microphone")
	bool bPushToTalkActive = false;

public:
	virtual void SetupInputComponent() override;

private:
	bool bOptionsMenuIsOn = false;

protected:
	UFUNCTION(BlueprintCallable)
	void ToggleOptions();
	
	void PushToTalkStarted();
	void PushToTalkCompleted();

public:
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	bool IsPushToTalkActive() const;

	UFUNCTION(BlueprintCallable)
	void SetPushToTalkActive(bool IsActive);

public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RequestRespawn();
	void Server_RequestRespawn_Implementation();


protected:
	UFUNCTION(BlueprintNativeEvent)
	void ToggleOptionsMenu(bool On);
	void ToggleOptionsMenu_Implementation(bool On);

protected:

	virtual void PawnLeavingGame() override;
};
