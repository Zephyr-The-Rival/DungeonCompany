// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemData.generated.h"

/**
 * 
 */

class AWorldItem;

UCLASS(Blueprintable)
class UE_DUNGEONCOMPANY_API UItemData : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AWorldItem> MyWorldItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UObject> AnimationBlueprintClass;

protected:


};
