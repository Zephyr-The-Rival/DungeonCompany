// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/AIEntity.h"
#include "FunGuy.generated.h"

/**
 * 
 */
class UStaticMeshComponent;
class USphereComponent;
class APlayerCharacter;
class UNiagaraComponent;
class UDebuffPoisonGas;

UCLASS()
class UE_DUNGEONCOMPANY_API AFunGuy : public AAIEntity
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CloudMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* CloudSphere;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* CloudNiagara;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* FloorMesh;

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

	UPROPERTY(EditAnywhere, Category = "Balancing|Cloud")
	float SporeProduceAge = 190.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Cloud")
	float StartCloudRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Cloud")
	float CloudUpdateInterval = 1.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Cloud")
	float CloudSizeFactor = 3.f;

	UPROPERTY(EditAnywhere, Category = "Balancing")
	float AgingMultiplier = 1.f;

	UPROPERTY(EditAnywhere, Category = "Balancing")
	float PulseFrequency = 0.5f;

public:
	AFunGuy();

private:
	UMaterialInstanceDynamic* DynamicMaterial;

	FTimerHandle UpdateTimerHandle;

	float LastNiagaraScaleUpdate = 0.f;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

protected: 
	void CalculateCloudStart();
	void UpdateCloud();
	void ResetCloudSpawnRate() const;

private:
	UPROPERTY(EditAnywhere, Category = "Balancing")
	float WobblingScale = 0.2f;

	bool bLifted = false;

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:
	static TMap<APlayerCharacter*, FTimerHandle> PlayerTimerHandles;
	static TMap<APlayerCharacter*, uint16> PlayerCloudOverlapCount;

	UPROPERTY(EditAnywhere, Category="Balancing|PlayerEffects")
	float SafeTime = 5.f;

	float CloudSizeMultiplierPerUpdate;

protected:
	UFUNCTION()
	void OnCloudBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnCloudEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void OnSafeTimerElapsed(APlayerCharacter* PlayerCharacter) const;

public:
	virtual void OnDeath_Implementation() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Buff/Debuff")
	TSubclassOf<UDebuffPoisonGas> PoisonGasDebuffClass;
};
