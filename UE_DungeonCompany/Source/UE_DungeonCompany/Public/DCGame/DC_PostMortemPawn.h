// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DC_PostMortemPawn.generated.h"

class APlayerCharacter;
class ADC_Entity;

class UVOIPTalker;

UCLASS()
class UE_DUNGEONCOMPANY_API ADC_PostMortemPawn : public APawn
{
	GENERATED_BODY()

private:
	TArray<APlayerCharacter*> PlayerCharacters;
	int SpecatingPlayerIndex = -1;
	
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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void SpectatePlayer(APlayerCharacter* InSpecatingPlayer);

	UFUNCTION(Server, Unreliable)
	void Server_SpectatePlayer(APlayerCharacter* InSpecatingPlayer);
	void Server_SpectatePlayer_Implementation(APlayerCharacter* InSpecatingPlayer);


	void SpectateLastPlayer();
	void SpectateNextPlayer();

};
