// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/ClimbingHook/Hook.h"
#include "ClimbingHook.generated.h"

/**
 * 
 */

 class ARope;

UCLASS()
class UE_DUNGEONCOMPANY_API AClimbingHook : public AHook
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARope> RopeClass;

	UPROPERTY(EditAnywhere)
	float RopeAutoFreezeTime = 7.f;

public:
	AClimbingHook();

private:
	ARope* SpawnedRope;
	float RopeLifetime = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void UpdateHookBehavior() override;

public:
	virtual void Tick(float DeltaTime) override;

};
