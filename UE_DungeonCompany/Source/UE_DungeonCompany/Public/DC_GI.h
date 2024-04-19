// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DC_GI.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UDC_GI : public UGameInstance
{
	GENERATED_BODY()

public: 
	UDC_GI();
protected:

	virtual void Init() override;

};
