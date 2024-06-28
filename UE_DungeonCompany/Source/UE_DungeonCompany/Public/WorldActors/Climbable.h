// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Climbable.generated.h"

UCLASS(Abstract)
class UE_DUNGEONCOMPANY_API AClimbable : public AActor
{
	GENERATED_BODY()

private:
	TArray<UShapeComponent*> ClimbVolumes;

	TMap<ACharacter*, TArray<UPrimitiveComponent*>> VolumesOverlappingCharacter;
	
public:	
	AClimbable();

protected:
	virtual void BeginPlay() override;

private:
	bool bClimbVolumeSetupCompleted = false;

protected:
	void RegisterClimbVolume(UShapeComponent* InClimbVolume);

	void SetupClimbVolumes();

	UFUNCTION()
	void OnClimbVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnClimbVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	//Climbable Interface
	virtual FVector GetLocationAtZ(double Z) const;
	virtual FVector GetUpVectorAtZ(double Z) const;
	virtual double GetClimbRotationYaw() const;

	virtual FVector GetLowerEndLocation() const;
	virtual FVector GetUpperEndLocation() const;

};
