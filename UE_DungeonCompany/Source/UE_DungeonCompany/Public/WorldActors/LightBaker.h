// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightBaker.generated.h"

UCLASS()
class UE_DUNGEONCOMPANY_API ALightBaker : public AActor
{
	GENERATED_BODY()
	

protected:

	UFUNCTION(BlueprintCallable)
	void SetShadowProjectSettings(bool bOn);
};
