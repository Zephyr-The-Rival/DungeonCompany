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

UCLASS()
class UE_DUNGEONCOMPANY_API ADC_PostMortemPawn : public APawn
{
	GENERATED_BODY()

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

	UFUNCTION()
	void OnPlayerDied(ADC_Entity* DeadPlayer);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

protected:
	UFUNCTION(Client, Reliable)
	void Client_PossessedBy(AController* NewController);
	void Client_PossessedBy_Implementation(AController* NewController);

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

	void SpectatePlayer(APlayerCharacter* InSpecatingPlayer);

protected:
	UFUNCTION(Server, Unreliable)
	void Server_SpectatePlayer(APlayerCharacter* InSpecatingPlayer);
	void Server_SpectatePlayer_Implementation(APlayerCharacter* InSpecatingPlayer);

	UFUNCTION(Server, Unreliable)
	void Client_SpectatePlayer(APlayerCharacter* InSpecatingPlayer);
	void Client_SpectatePlayer_Implementation(APlayerCharacter* InSpecatingPlayer);

	UFUNCTION()
	void OnInputDeviceChanged(bool IsUsingGamepad);

private:
	void (*LookFunction)(const FInputActionValue& Value, APawn* Player) = &UInputFunctionLibrary::LookGamepad;

protected:
	void Look(const FInputActionValue& Value);
	void NoLook();

	void SpectateLastPlayer();
	void SpectateNextPlayer();

};
