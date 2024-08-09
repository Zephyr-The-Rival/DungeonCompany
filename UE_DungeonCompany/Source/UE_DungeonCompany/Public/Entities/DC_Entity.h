// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DC_Entity.generated.h"

class UBuffDebuffBase;
class UNiagaraSystem;
UCLASS()
class UE_DUNGEONCOMPANY_API ADC_Entity : public ACharacter
{
	GENERATED_BODY()

protected:	
	UPROPERTY(EditAnywhere,	BlueprintGetter = GetMaxHealth, Category = "Balancing|Health")
	float MaxHP = 100.f;

	UPROPERTY(EditAnywhere, ReplicatedUsing = CheckIfDead, BlueprintGetter = GetHealth)
	float HP = 100.f;

public:
	ADC_Entity();
	ADC_Entity(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	float GetHealth() const { return HP; }

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	float GetMaxHealth() const { return MaxHP; }

	UFUNCTION(BlueprintPure, BlueprintCallable)
	inline bool IsDead() const { return HP <= 0.f; }

	using Super::TakeDamage;
	virtual void TakeDamage(float Damage);
	
	UFUNCTION(NetMulticast, Unreliable)
	void SpawnHitEffect(USceneComponent* hitComponent, FName BoneName, FVector hitPoint, FVector HitNormal);
	void SpawnHitEffect_Implementation(USceneComponent* hitComponent, FName BoneName, FVector hitPoint, FVector HitNormal);

protected:
	virtual void OnTookDamage();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* bloodEffect;

	UFUNCTION()
	void CheckIfDead();

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnDeath();
	virtual void OnDeath_Implementation();

	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEntityDeath, ADC_Entity*, DeadPlayer);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEntityDeath OnEntityDeath;

public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	UBuffDebuffBase* AddBuffOrDebuff(TSubclassOf<UBuffDebuffBase> BuffDebuffClass, float ActiveTime = 0.f);
	void RemoveBuffOrDebuff(TSubclassOf<UBuffDebuffBase> BuffDebuffClass);

	bool HasBuffOrDebuffApplied(TSubclassOf<UBuffDebuffBase> BuffDebuffClass) const;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* TakeDamageSound;
public:
	
	UFUNCTION(NetMulticast, Unreliable)
	void SpawnTakeDamageSound();
};
