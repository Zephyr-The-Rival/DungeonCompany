// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/WorldItem.h"
#include "Hook.generated.h"

/**
 * 
 */
UENUM()
enum class EHookState : uint8
{
	InHand = 0 UMETA(DisplayName = "In Hand"),
	InWorldInactive = 1 UMETA(DisplayName = "In World Inactive"),
	InWorldActive = 2 UMETA(DisplayName = "In World Active"),
	InWorldAttached = 3 UMETA(DisplayName = "In World Attached"),
	InHandAfterThrow = 4 UMETA(DisplayName = "In Hand after throw"),
};

class USphereComponent;

UCLASS()
class UE_DUNGEONCOMPANY_API AHook : public AWorldItem
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* HookMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* EasyInteract;

public:
	inline UStaticMeshComponent* GetHookMesh() const { return HookMesh; }

private:
	UPROPERTY(EditAnywhere, Category = "Balancing")
	float MaxAttachDistance = 150.f;

	UPROPERTY(EditAnywhere, ReplicatedUsing = UpdateState)
	EHookState State = EHookState::InWorldInactive;

protected:
	UFUNCTION()
	void UpdateState();

	virtual void UpdateHookBehavior();

public:
	inline float GetMaxAttachDistance() const { return MaxAttachDistance; }

	inline EHookState GetHookState() const { return State; }

public:
	AHook();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TriggerPrimaryAction_Implementation(APlayerCharacter* User) override;
	virtual void TriggerSecondaryAction_Implementation(APlayerCharacter* User) override;

	virtual void OnHoldingInHand_Implementation(bool LocallyControlled) override;

protected:
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_ThrowHook(FVector Direction);
	void Multicast_ThrowHook_Implementation(FVector Direction);

protected:
	UFUNCTION()
	void OnHookHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	               FVector NormalImpulse, const FHitResult& Hit);

protected:
	FHitResult GetAttachHit(APlayerCharacter* User);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable)
	void HookLetGo(APlayerCharacter* User);

	UFUNCTION(BlueprintCallable)
	void OnHookThrown(APlayerCharacter* User);
};
