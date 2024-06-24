// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Rope.generated.h"

class APhysicsConstraintActor;

UCLASS()
class UE_DUNGEONCOMPANY_API ARope : public AActor, public IInteractable
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* RopeMesh;

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

	int setupTries = 0;

	void SetupActorAttachment();

public:
	virtual void Interact(APawn* InteractingPawn) override;
	virtual void AuthorityInteract(APawn* InteractingPawn) override;

	void GetEdgeLocations(TArray<FVector>& Out);

};
