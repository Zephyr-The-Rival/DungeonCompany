// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/WorldItem.h"
#include "WorldCurrency.generated.h"

/**
 * 
 */
class AWorldCurrency;
UCLASS()
class UE_DUNGEONCOMPANY_API AWorldCurrency : public AWorldItem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite,Replicated)
	int32 Value=1;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
