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
class UDebuffDisableMovement;
class UDebuffBlockInputs;
class UDebuffMuffledVoice;
class UDebuffImpairedVision;

UCLASS()
class UE_DUNGEONCOMPANY_API AQuasoSnake : public AAIEntity
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	USphereComponent* EyeCollision;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TopCaveMesh;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BottomCaveMesh;

	UPROPERTY(EditAnywhere)
	UAnimationAsset* FirstPersonChoke;

	UPROPERTY(EditAnywhere, Category="Balancing")
	float DeathSeconds = 30.f;

	UPROPERTY(EditAnywhere, Category="Balancing")
	float WindUpSeconds = 1.f;

public:
	AQuasoSnake();

protected:
	virtual void BeginPlay() override;
	virtual void OnAnimationFlagUpdated_Implementation() override;

private:
	bool bInAttack = false;

public:
	virtual void AttackPlayer(APlayerCharacter* PlayerAttacking) override;

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
	USkeletalMeshComponent* FirstPersonAttach;

public:
	inline APlayerCharacter* GetPlayerAttachedTo() const { return PlayerAttachedTo; }

public:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	           FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_OnAttachedToPlayer(APlayerCharacter* AttachedPlayer);
	void Multicast_OnAttachedToPlayer_Implementation(APlayerCharacter* AttachedPlayer);

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
	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWantsToReturnToVolume);

	FOnWantsToReturnToVolume OnWantsToReturnToVolume;

public:
	void ReturnToVolume();

protected: //blueprint debuffs

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Buff/Debuff")
	TSubclassOf<UDebuffDisableMovement> DisableMovementDebuff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Buff/Debuff")
	TSubclassOf<UDebuffBlockInputs> BlockInputsDebuff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Buff/Debuff")
	TSubclassOf<UDebuffMuffledVoice> MuffledVoiceDebuff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Buff/Debuff")
	TSubclassOf<UDebuffImpairedVision> ImpairedVisionDebuff;

public:
	inline bool IsAttachedToPlayer() const { return (AnimationFlags & FLAG_Custom_0) != 0; }
	void SetIsAttachedToPlayer(bool InIsAttached);

	inline bool IsLurking() const { return (AnimationFlags & FLAG_Custom_1) != 0; }
	void SetIsLurking(bool InIsLurking);
};
