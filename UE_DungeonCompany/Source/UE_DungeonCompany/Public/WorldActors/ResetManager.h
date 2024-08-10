// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResetManager.generated.h"

UCLASS()
class UE_DUNGEONCOMPANY_API AResetManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AResetManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	void Server_ResetDungeon();

protected:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BP_ResetDungeon();
	void BP_ResetDungeon_Implementation();
};
