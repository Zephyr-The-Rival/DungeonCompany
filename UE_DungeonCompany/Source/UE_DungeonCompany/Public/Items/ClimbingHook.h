// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/WorldItem.h"
#include "ClimbingHook.generated.h"

/**
 * 
 */
 class UCableComponent;
 class UStaticMeshComponent;
 class USphereComponent;

UCLASS()
class UE_DUNGEONCOMPANY_API AClimbingHook : public AWorldItem
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Balancing")
	float MaxAttachDistance = 150.f;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* HookMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* EasyInteract;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AClimbingHook> BP_StaticClass;

private:
	APlayerCharacter* CharacterHoldingPartnerHook;

	UCableComponent* Rope;

	bool bAttached = false;

public:
	AClimbingHook();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Interact(APawn* InteractingPawn) override;

	virtual void TriggerPrimaryAction_Implementation(APlayerCharacter* User) override;
	virtual void TriggerLocalPrimaryAction_Implementation(APlayerCharacter* User) override;

	virtual void TriggerSecondaryAction_Implementation(APlayerCharacter* User) override;
	virtual void TriggerLocalSecondaryAction_Implementation(APlayerCharacter* User) override;

	virtual void OnHoldingInHand_Implementation(bool LocallyControlled) override;

protected:
	FVector GetAttachLocation(APlayerCharacter* User);

	void AttachRopeToHooks(AClimbingHook* From, AClimbingHook* To);

};
