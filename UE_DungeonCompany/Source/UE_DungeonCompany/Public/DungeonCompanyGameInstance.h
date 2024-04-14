// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DungeonCompanyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UDungeonCompanyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public: 
	UDungeonCompanyGameInstance();
protected:

	virtual void Init() override;

};
