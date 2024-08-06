// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuffSystem/BuffDebuffBase.h"
#include "DebuffImpairedVision.generated.h"

class UCameraComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class UE_DUNGEONCOMPANY_API UDebuffImpairedVision : public UBuffDebuffBase
{
	GENERATED_BODY()


protected:
	virtual void LocalApply() override;

	virtual void LocalRemove() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeightedBlendable PostProcessMaterial;

	UFUNCTION(BlueprintNativeEvent)
	void SmoothApply(UCameraComponent* Camera);
	void SmoothApply_Implementation(UCameraComponent* Camera);
};
