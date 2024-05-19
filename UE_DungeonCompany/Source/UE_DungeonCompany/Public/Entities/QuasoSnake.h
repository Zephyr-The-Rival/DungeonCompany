// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/AIEntity.h"
#include "QuasoSnake.generated.h"

/**
 * 
 */
class USplineComponent;

UCLASS()
class UE_DUNGEONCOMPANY_API AQuasoSnake : public AAIEntity
{
	GENERATED_BODY()
private:

	UPROPERTY(EditAnywhere, Category="Balancing")
	float WindUpTime = 2.f;

	USplineComponent* AttackSpline;

	bool bInAttack = false;
	float AttackTime = 0.f;

public:
	AQuasoSnake();

protected:
	virtual void BeginPlay() override;

public:
	virtual void AttackPlayer(APlayerCharacter* TargetPlayer) override;

private:
	FTimerHandle LaunchHandle;
	bool bLaunched = false;

	UPROPERTY(EditAnywhere)
	float LaunchUpdateInterval = 0.01f;

protected:
	void LaunchAtActor(AActor* Actor);
	void CalculateLaunchSplineToActor(AActor* Actor);

public:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_OnAttachedToPlayer();
	void Multicast_OnAttachedToPlayer_Implementation();
	
};
