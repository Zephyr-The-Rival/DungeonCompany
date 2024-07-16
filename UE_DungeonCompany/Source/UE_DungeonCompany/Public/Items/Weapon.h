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
    void DealHits(UPrimitiveComponent* WeaponCollision, const TArray<FVector>& TraceStarts, const TArray<FVector>&  TraceEnd);
	void DealHits_Implementation(UPrimitiveComponent* WeaponCollision, const TArray<FVector>& TraceStarts, const TArray<FVector>&  TraceEnd);

	virtual void TriggerPrimaryAction_Implementation(APlayerCharacter* User) override;
	virtual void TriggerLocalPrimaryAction_Implementation(APlayerCharacter* User) override;

};
