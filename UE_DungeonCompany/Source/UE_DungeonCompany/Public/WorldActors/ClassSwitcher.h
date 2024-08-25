// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicButton.h"
#include "GameFramework/Actor.h"
#include "ClassSwitcher.generated.h"

UCLASS()
class UE_DUNGEONCOMPANY_API AClassSwitcher : public ABasicButton
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AClassSwitcher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	void OnHovered(APlayerCharacter* PlayerCharacter) override;
};
