// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldActors/Climbable.h"
#include "Interactable.h"
#include "Rope.generated.h"

class UPoseableMeshComponent;
class APhysicsConstraintActor;

UCLASS()
class UE_DUNGEONCOMPANY_API ARope : public AClimbable, public IInteractable
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* RopeMesh;

	UPoseableMeshComponent* FixedRopeMesh;

private:
	APhysicsConstraintActor* PhysicsConstraintActor;

public:
	inline USkeletalMeshComponent* GetRopeMesh() const { return RopeMesh; }
	
public:	
	ARope();

private:
	FTimerHandle CheckOwnerHandle;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SetupActorAttachment();

private:
	TArray<FName> BoneNames;
	FTimerHandle VelocityCheckTimer;

	float SettledMaxRopeDelta = 1.f;

	bool bStartedMoving = false;
protected:
	TArray<FVector> LastRopeLocations;

public:
	virtual void Tick(float DeltaSeconds) override;
	bool IsRopeSettled(float DeltaTime, bool bIgnoreIfMovedBefore = false);

public:
	virtual void Interact(APawn* InteractingPawn) override;
	virtual void AuthorityInteract(APawn* InteractingPawn) override;

protected:
	UFUNCTION()
	void StoppedInteracting(APlayerCharacter* PlayerCharacter);

public:
	void GetRopeTransforms(USkinnedMeshComponent* InRopeMesh, TArray<FTransform>& OutTransforms);
	void GetBoneLocations(TArray<FVector>& OutLocations);

	void GetEdgeLocations(TArray<FVector>& OutLocations);
	FVector GetWorldLocationAtDistance(float Distance);

	/* This function only works when called from the Server.*/
	void FreezeAndReplicate();

protected:
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SetTransformsAndFreeze(const TArray<FTransform>& RopeTransforms);
	void Multicast_SetTransformsAndFreeze_Implementation(const TArray<FTransform>& RopeTransforms);

private:
	mutable int CachedBoneIndex = -1;

	UPROPERTY(Transient)
	float BoneLength;

protected:
	FName GetBoneNameNearestToZ(double Z) const;

	int GetBoneIndexNearestToZLinear(double Z) const;
	int GetBoneIndexNearestToZBinary(double Z) const;

	FVector GetBoneUpVectorByName(FName BoneName) const;

private:
	class USplineComponent* SplineComponent;

public:
	virtual FVector GetLocationAtZ(double Z) const;
	virtual FVector GetUpVectorAtZ(double Z) const;
	virtual double GetClimbRotationYaw(AActor* ClimbingActor) const;

protected:
	virtual void CalculateLowerEndLocation() const override;

};
