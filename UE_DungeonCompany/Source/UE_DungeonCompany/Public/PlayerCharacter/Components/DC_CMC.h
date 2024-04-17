// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DC_CMC.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UDC_CMC : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual bool CanAttemptJump() const override;
	
};
