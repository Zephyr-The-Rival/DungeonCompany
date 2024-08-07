// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGM.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API AMainMenuGM : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainMenuGM();

protected:
	virtual void BeginPlay() override;
	
};
