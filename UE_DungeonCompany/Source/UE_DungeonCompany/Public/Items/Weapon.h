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

UENUM(BlueprintType)
enum EWeaponType
{
	None UMETA(DisplayName = "None"),
	Blade UMETA(DisplayName = "Blade"),
	Tool UMETA(DisplayName = "Tool")
};

class APlayerCharacter;

UCLASS()
class UE_DUNGEONCOMPANY_API AWeapon : public AWorldItem
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EWeaponType> WeaponType = None;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure)
	FWeaponInfo GetWeaponInfo();

	inline EWeaponType GetWeaponType() const { return WeaponType; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debugging")
	bool bDrawDebugLinesOnAttack = false;

public:
	void DealHits(FWeaponInfo WeaponInfo);


	virtual void TriggerPrimaryAction_Implementation(APlayerCharacter* User) override;
	virtual void TriggerLocalPrimaryAction_Implementation(APlayerCharacter* User) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Balancing")
	float Damage = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Balancing")
	float StaminaCost = 3;

public:
	float GetStaminaCost() const { return this->StaminaCost; };
};
