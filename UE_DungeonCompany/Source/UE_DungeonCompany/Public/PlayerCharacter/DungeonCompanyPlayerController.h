// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DungeonCompanyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API ADungeonCompanyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;
};
