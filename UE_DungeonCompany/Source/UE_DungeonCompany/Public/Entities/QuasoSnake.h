// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/AIEntity.h"
#include "QuasoSnake.generated.h"

/**
 * 
 */
class USphereComponent;
class USplineComponent;
class APlayerCharacter;
class UBuffDebuffBase;

UCLASS()
class UE_DUNGEONCOMPANY_API AQuasoSnake : public AAIEntity
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	USphereComponent* EyeCollision;

	UPROPERTY(EditAnywhere, Category="Balancing")
	float DeathSeconds = 30.f;
	
	UPROPERTY(EditAnywhere, Category="Balancing")
	float WindUpSeconds = 1.f;

public:
	AQuasoSnake();

protected:
	virtual void BeginPlay() override;

private:
	bool bInAttack = false;

public:
	virtual void AttackPlayer(APlayerCharacter* TargetPlayer) override;

private:
	FTimerHandle LaunchHandle;
	bool bLaunched = false;

	UPROPERTY(EditAnywhere)
	float LaunchUpdateInterval = 0.01f;

	USplineComponent* AttackSpline;

	float AttackTime = 0.f;

protected:
	void LaunchAtActor(AActor* Actor);
	void CalculateLaunchSplineToActor(AActor* Actor);

private:
	APlayerCharacter* PlayerAttachedTo;

public:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_OnAttachedToPlayer();
	void Multicast_OnAttachedToPlayer_Implementation(); 

	virtual void OnDeath_Implementation() override;

private:
	int CurrentStage = -1;
	FTimerHandle StageProgressHandle;

	TArray<UBuffDebuffBase*> AppliedDebuffs;

protected:
	void ProgressStage();
	void ResetPlayerEffects();

	void DetachFromPlayer();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_OnDetachedFromPlayer();
	void Multicast_OnDetachedFromPlayer_Implementation();

public:
	static AQuasoSnake* Spawn(UWorld* World);
	
};
