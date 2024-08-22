// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputFunctionLibrary.h"
#include "DC_PostMortemPawn.generated.h"

class APlayerCharacter;
class ADC_Entity;

class UVOIPTalker;
class UInputMappingContext;
class UInputAction;
class USpectatorHud;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpectatingSwitch, ADC_PostMortemPawn*, PostMortemPawn);

UCLASS()
class UE_DUNGEONCOMPANY_API ADC_PostMortemPawn : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Balancing")
	float SpectateSwitchAfterDeathDelay = 4.f;

private:
	TArray<APlayerCharacter*> PlayerCharacters;
	int SpecatingPlayerIndex = 0;
	
	UVOIPTalker* VOIPTalker;

public:
	// Sets default values for this pawn's properties
	ADC_PostMortemPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnPlayerDied(ADC_Entity* DeadPlayer);

	void StopSpectatingPlayer(APlayerCharacter* InPlayer);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Restart() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Input | Mapping")
	UInputMappingContext* PMPawnInputMapping;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* SpectateLastAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* SpectateNextAction;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Client, Unreliable)
	void Client_ForceSpectatePlayer(APlayerCharacter* InSpectatingPlayer);
	void Client_ForceSpectatePlayer_Implementation(APlayerCharacter* InSpectatingPlayer);

protected:
	void SpectatePlayer(APlayerCharacter* InSpectatingPlayer);
	bool IsSpectatingPlayer(APlayerCharacter* InPlayer);

	UFUNCTION()
	void OnInputDeviceChanged(bool IsUsingGamepad);

private:
	void (*LookFunction)(const FVector2d& Value, APawn* Player) = &UInputFunctionLibrary::LookGamepad;

protected:
	void Look(const FInputActionValue& Value);
	void NoLook();

	void SpectatePreviousPlayer();
	void SpectateNextPlayer();


public:
	void CreateSpectatorHud();

	USpectatorHud* MySpectatorHud;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USpectatorHud> SpectatorHudClass;

	UPROPERTY(BlueprintAssignable)
	FOnSpectatingSwitch OnSpectatingSwitch;

protected:

	virtual void Destroyed() override;
};
