// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightBaker.generated.h"

UCLASS()
class UE_DUNGEONCOMPANY_API ALightBaker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightBaker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UFUNCTION(BlueprintCallable)
	void SetShadowProjectSettings(bool bOn);
};
