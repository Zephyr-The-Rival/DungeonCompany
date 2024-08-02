// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpectatorHud.generated.h"

/**
 * 
 */

class ADC_PostMortemPawn;



UCLASS()
class UE_DUNGEONCOMPANY_API USpectatorHud : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ADC_PostMortemPawn* MySpectatorPawn;
};
