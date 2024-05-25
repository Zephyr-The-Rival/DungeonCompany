// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/WorldItem.h"
#include "Weapon.generated.h"


/**
 * 
 */


USTRUCT(BlueprintType)
struct FWeaponHit
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	AActor* HitActor;

	UPROPERTY(BlueprintReadWrite)
	bool bWeakspotHit;
};

class APlayerCharacter;

UCLASS()
class UE_DUNGEONCOMPANY_API AWeapon : public AWorldItem
{
	GENERATED_BODY()
	

public:

	//UFUNCTION(,BlueprintImplementableEvent)
	//TArray<AActor*> GetHitActors();

	UFUNCTION(BlueprintNativeEvent)
    TArray<UPrimitiveComponent*> GetHitComponents();
    virtual TArray<UPrimitiveComponent*> GetHitComponents_Implementation();
	
	UFUNCTION(BlueprintNativeEvent)
    bool IsWeakspotHit();
    bool IsWeakspotHit_Implementation();

	UFUNCTION(BlueprintNativeEvent)
    TArray<FWeaponHit> GetHits();
	TArray<FWeaponHit> GetHits_Implementation();

	void TriggerPrimaryAction_Implementation(APlayerCharacter* user) override;

};
