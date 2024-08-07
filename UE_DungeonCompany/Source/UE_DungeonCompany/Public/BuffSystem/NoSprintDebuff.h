// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuffDebuffBase.h"
#include "UObject/Object.h"
#include "NoSprintDebuff.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UE_DUNGEONCOMPANY_API UNoSprintDebuff : public UBuffDebuffBase
{
	
	GENERATED_BODY()

protected:

	void LocalApply() override;
	void LocalRemove() override;
};
