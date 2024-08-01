// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldActors/Climbable.h"
#include "Interactable.h"
#include "Ladder.generated.h"

class UBoxComponent;
class APlayerCharacter;

UCLASS()
class UE_DUNGEONCOMPANY_API ALadder : public AClimbable, public IInteractable
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UBoxComponent* InteractVolume;

	UPROPERTY()
	UBoxComponent* ClimbVolume;

	UPROPERTY(EditAnywhere)
	UBoxComponent* EasyInteractBox;

	UPROPERTY(EditAnywhere, Category = Balancing)
	float Height = 100.f;

	UPROPERTY(EditAnywhere, Category = Interaction)
	FVector2D InteractionArea = FVector2D(10, 20);

	UPROPERTY(EditAnywhere, Category = Interaction)
	float InteractionVolumeHeightBonus = 20.f;

	UPROPERTY(EditAnywhere, Category = Interaction)
	bool bIgnoreInteractionVolume = false;

	UPROPERTY(EditAnywhere, Category = Interaction)
	FVector2D ClimbArea = FVector2D(25, 25);

	UPROPERTY(EditAnywhere, Category = Interaction)
	FVector2D EasyInteractArea = FVector2D(5, 20);

public:
	virtual float GetDistanceAtLocation(FVector ClimbingActorLocation) const override;
	virtual FVector GetLocationAtDistance(float Distance) const override;

	/*This doesn't set the height in runtime. Set this in the constructor or before calling the construction script of the Ladder class.*/
	void SetHeight(float InHeight); 

	void SetIgnoreInteractionVolume(bool InIgnore);

protected:
	virtual void CalculateUpperEndLocation() const override;

public:	
	ALadder();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	void CalculateLadderBoxExtents();

public:
	virtual void Interact(APawn* InteractingPawn) override;

private:	
	bool bInInteractionVolume = false;

protected:
	UFUNCTION()
	void OnInteractVolumeEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractVolumeLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	APlayerCharacter* LocalPlayerOnLadder;

protected:
	UFUNCTION()
	void StoppedInteracting(APlayerCharacter* PlayerCharacter);

public:
	virtual void OnHovered(APlayerCharacter* PlayerCharacter);

public:
	void PlaceLadder(const FVector& From, const FVector& To, const FVector& ForwardVector);

};
