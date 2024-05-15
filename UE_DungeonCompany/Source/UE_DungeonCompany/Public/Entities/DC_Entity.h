// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DC_Entity.generated.h"

UCLASS()
class UE_DUNGEONCOMPANY_API ADC_Entity : public ACharacter
{
	GENERATED_BODY()

protected:	
	UPROPERTY(EditAnywhere,	BlueprintGetter = GetMaxHealth, Category = "Balancing|Health")
	float MaxHP = 100.f;

	UPROPERTY(EditAnywhere, Replicated, BlueprintGetter = GetHealth)
	float HP = 100.f;

public:
	ADC_Entity();
	ADC_Entity(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	float GetHealth() const { return HP; }

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	float GetMaxHealth() const { return MaxHP; }

	using Super::TakeDamage;
	virtual void TakeDamage(float Damage);

	UFUNCTION(BlueprintNativeEvent)
	void OnDeath();
	virtual void OnDeath_Implementation();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

};
