 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/WorldItem.h"
#include "BackPack.generated.h"

/**
 * 
 */
class UItemData;
UCLASS()
class UE_DUNGEONCOMPANY_API ABackPack : public AWorldItem
{
	GENERATED_BODY()
	

public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UItemData>> Items;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	TArray<FString> ItemDatas;

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
