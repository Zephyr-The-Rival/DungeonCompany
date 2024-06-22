// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/WorldItem.h"
#include "Hook.generated.h"

/**
 * 
 */

class USphereComponent;

UCLASS()
class UE_DUNGEONCOMPANY_API AHook : public AWorldItem
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* HookMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* EasyInteract;

private:
	UPROPERTY(EditAnywhere, Category = "Balancing")
	float MaxAttachDistance = 150.f;

public:
	inline float GetMaxAttachDistance() const { return MaxAttachDistance; }

public:
	AHook();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Replicated)
	bool bAttached = true;

public:
	virtual void TriggerPrimaryAction_Implementation(APlayerCharacter* User) override;

	virtual void TriggerSecondaryAction_Implementation(APlayerCharacter* User) override;

	virtual void OnHoldingInHand_Implementation(bool LocallyControlled) override;

protected:
	UFUNCTION()
	void OnHookHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	FHitResult GetAttachHit(APlayerCharacter* User);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
