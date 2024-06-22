// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/ClimbingHook/Hook.h"
#include "ClimbingHook.generated.h"

/**
 * 
 */

UENUM()
enum class ClimbingHookState : uint8 {
     InHand = 0 UMETA(DisplayName = "In Hand"),
     InWorldInactive = 1  UMETA(DisplayName = "In World Inactive"),
     InWorldActive = 2     UMETA(DisplayName = "In World Active"),
};

 class ARope;

UCLASS()
class UE_DUNGEONCOMPANY_API AClimbingHook : public AHook
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ARope> RopeClass;

private:
	UPROPERTY(EditAnywhere)
	ClimbingHookState State = ClimbingHookState::InWorldInactive;

public:
	AClimbingHook();

private:
	ARope* SpawnedRope;

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
