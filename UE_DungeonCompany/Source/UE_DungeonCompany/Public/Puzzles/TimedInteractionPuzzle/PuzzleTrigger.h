// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleTrigger.generated.h"

UCLASS()
class UE_DUNGEONCOMPANY_API APuzzleTrigger : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Balancing|Puzzle")
	uint8 Channel = 0;
	
public:	
	// Sets default values for this actor's properties
	APuzzleTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	uint8 GetChannel() const { return Channel; }

public:
	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggered, APuzzleTrigger*, TriggeredPuzzleTrigger);

	FOnTriggered OnTriggered;

	void Trigger();

};
