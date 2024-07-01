// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuffSystem/BuffDebuffBase.h"
#include "DebuffImpairedVision.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UDebuffImpairedVision : public UBuffDebuffBase
{
	GENERATED_BODY()

private:
	UMaterialInstance* ImpairedVisionMaterial;

protected:
	virtual void LocalApply() override;

	virtual void LocalRemove() override;
	
};
