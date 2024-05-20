// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/WorldItem.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API AWeapon : public AWorldItem
{
	GENERATED_BODY()
	

public:

	//UFUNCTION(,BlueprintImplementableEvent)
	//TArray<AActor*> GetHitActors();

	UFUNCTION(BlueprintNativeEvent)
    TArray<AActor*> GetHitActors();
    virtual TArray<AActor*> GetHitActors_Implementation();
	

};
