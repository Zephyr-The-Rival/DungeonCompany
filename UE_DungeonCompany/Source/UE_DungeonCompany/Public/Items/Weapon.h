// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/WorldItem.h"
#include "Weapon.generated.h"


/**
 * 
 */

USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> TraceStarts;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> TraceEnds;
};
class APlayerCharacter;

UCLASS()
class UE_DUNGEONCOMPANY_API AWeapon : public AWorldItem
{
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;
	
	

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure)
	FWeaponInfo GetWeaponInfo();

	
    void DealHits(FWeaponInfo WeaponInfo);
    

	virtual void TriggerPrimaryAction_Implementation(APlayerCharacter* User) override;
	virtual void TriggerLocalPrimaryAction_Implementation(APlayerCharacter* User) override;

};
