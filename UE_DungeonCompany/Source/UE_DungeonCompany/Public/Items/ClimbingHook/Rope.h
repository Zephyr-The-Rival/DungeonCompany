// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Rope.generated.h"

class APhysicsConstraintActor;
class USkeletalMeshComponent;

UCLASS()
class UE_DUNGEONCOMPANY_API ARope : public AActor, public IInteractable
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* RopeMesh;

	UPROPERTY(EditAnywhere)
	APhysicsConstraintActor* PhysicsConstraintActor;

private:
	UPROPERTY(Replicated)
	AActor* AttachingActor;

public:
	inline USkeletalMeshComponent* GetRopeMesh() const { return RopeMesh; }

	void SetAttachingActor(AActor* InActor);
	
public:	
	ARope();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Interact(APawn* InteractingPawn) override;
	virtual void AuthorityInteract(APawn* InteractingPawn) override;

	void GetEdgeLocations(TArray<FVector>& Out);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
