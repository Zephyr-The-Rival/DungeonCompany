// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuffSystem/BuffDebuffBase.h"
#include "DebuffDisableMovement.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UDebuffDisableMovement : public UBuffDebuffBase
{
	GENERATED_BODY()

private:
	EMovementMode OriginalMovementMode;

protected:
	virtual void Apply() override;

	virtual void Remove() override;
	
};
