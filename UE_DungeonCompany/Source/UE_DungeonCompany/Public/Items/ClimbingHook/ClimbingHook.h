// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/WorldItem.h"
#include "ClimbingHook.generated.h"

/**
 * 
 */

UENUM()
enum class ClimbingHookState : uint8 {
     InHand = 0 UMETA(DisplayName = "In Hand"),
     InWorldInactive = 1  UMETA(DisplayName = "In World Inactive"),
     InWorldActive = 2     UMETA(DisplayName = "In World Active"),
};

 class URopeEndComponent;
 class ALadder;

 class UStaticMeshComponent;
 class USphereComponent;
 class UCableComponent;
 class UPhysicsConstraintComponent;

UCLASS()
class UE_DUNGEONCOMPANY_API AClimbingHook : public AWorldItem
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Balancing")
	float MaxAttachDistance = 150.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Rope")
	float RopeLength = 2500.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Rope")
	float RopeWidth = 10.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Rope")
	int32 RopeSegmentsNum = 100.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Ladder")
	float MinLadderHeight = 100.f;

	UPROPERTY(EditAnywhere)
	UStaticMesh* RopeEndMesh;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* HookMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* EasyInteract;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AClimbingHook> BP_StaticClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ALadder> LadderClass;

private:
	UPROPERTY(EditAnywhere)
	UPhysicsConstraintComponent* PhysicsConstraint;

	UPROPERTY(EditAnywhere)
	UCableComponent* Rope;

	UPROPERTY(EditAnywhere)
	URopeEndComponent* RopeEnd;

	UPROPERTY(EditAnywhere)
	ClimbingHookState State = ClimbingHookState::InWorldInactive;

	bool bAttached = false;
	FVector AttachNormal;

public:
	AClimbingHook();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TriggerPrimaryAction_Implementation(APlayerCharacter* User) override;
	virtual void TriggerLocalPrimaryAction_Implementation(APlayerCharacter* User) override;

	virtual void TriggerSecondaryAction_Implementation(APlayerCharacter* User) override;
	virtual void TriggerLocalSecondaryAction_Implementation(APlayerCharacter* User) override;

	virtual void OnHoldingInHand_Implementation(bool LocallyControlled) override;

protected:
	UFUNCTION()
	void OnHookHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	FHitResult GetAttachHit(APlayerCharacter* User);

private:
	TArray<ALadder*> CreatedLadders;

protected:
	void CreateLadders();

	void GetEdgeLocations(TArray<FVector>& Out);

};
