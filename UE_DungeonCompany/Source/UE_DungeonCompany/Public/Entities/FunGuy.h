// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/AIEntity.h"
#include "FunGuy.generated.h"

/**
 * 
 */
class USphereComponent;

UCLASS()
class UE_DUNGEONCOMPANY_API AFunGuy : public AAIEntity
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	USphereComponent* CloudSphere;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TempMesh;

private:
	UPROPERTY(EditAnywhere, Replicated)
	float AgeSeconds = 0.f;

	float AgeBonusScaleMultiplier = 0.01f;

	UPROPERTY(EditAnywhere, Category = "Balancing")
	float MaxSizeAgeSeconds = 300.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Liftoff")
	float LiftoffAge = 180.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Liftoff")
	float LiftoffHeight = 150.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Liftoff")
	float LiftoffTime = 10.f;

	UPROPERTY(EditAnywhere, Category = "Balancing")
	float CloudRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = "Balancing")
	float AgingMultiplier = 1.2f;

	UPROPERTY(EditAnywhere, Category = "Balancing")
	float PulseInterval = 2.f;

public:
	AFunGuy();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	
};
