// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/WorldItem.h"
#include "Weapon.generated.h"


/**
 * 
 */


class APlayerCharacter;

UCLASS()
class UE_DUNGEONCOMPANY_API AWeapon : public AWorldItem
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
public:

	UFUNCTION(BlueprintNativeEvent)
    void DealHits(UPrimitiveComponent* WeaponCollision, FVector traceStart, FVector TraceEnd);
	void DealHits_Implementation(UPrimitiveComponent* WeaponCollision, FVector traceStart, FVector TraceEnd);

	void TriggerPrimaryAction_Implementation(APlayerCharacter* user) override;

};
