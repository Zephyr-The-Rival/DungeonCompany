// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DungeonCompanyPlayerController.generated.h"

/**
 * 
 */
 class UVOIPTalker;

UCLASS()
class UE_DUNGEONCOMPANY_API ADungeonCompanyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADungeonCompanyPlayerController();

protected:
	virtual void BeginPlay() override;

	void SetupVOIPTalker();

private:
	UVOIPTalker* VOIPTalker;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* VoiceSA;

	USceneComponent* TalkerAttachedTo;

public:
	void AttachVOIPTalkerTo(USceneComponent* AttachTo);

};
