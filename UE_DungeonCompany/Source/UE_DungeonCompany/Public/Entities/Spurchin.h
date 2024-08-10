// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/AIEntity.h"
#include "Spurchin.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API ASpurchin : public AAIEntity
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Balancing")
	float HallwaySpeed = 280.f;

	UPROPERTY(EditAnywhere, Category = "Balancing")
	float OpenSpaceSpeed = 700.f;

	UPROPERTY(EditAnywhere, Category = "Balancing")
	float MaxHallwayWidth = 400.f;

public:
	ASpurchin();

private:
	UPROPERTY(Transient)
	float OriginalSightRadius;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

protected:
	bool IsInHallway();

	UFUNCTION(BlueprintCallable)
	void CheckSpace();
public:
	virtual void OnTargetingPlayer_Implementation(APlayerCharacter* Target) override;
	
};
