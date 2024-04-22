// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DC_PC.generated.h"

/**
 * 
 */
 class UVOIPTalker;
 class UPlayerHud;
UCLASS()
class UE_DUNGEONCOMPANY_API ADC_PC : public APlayerController
{
	GENERATED_BODY()

public:
	ADC_PC();
	UPlayerHud* GetMyPlayerHud() const { return MyPlayerHud; }
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPlayerHud> PlayerHudClass;

	UPlayerHud* MyPlayerHud;

};
