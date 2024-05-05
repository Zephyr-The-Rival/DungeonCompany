// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleObjective.generated.h"

class APuzzleTrigger;

UCLASS()
class UE_DUNGEONCOMPANY_API APuzzleObjective : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Balancing|Puzzle")
	uint8 Channel = 0;

	UPROPERTY(EditAnywhere, Category = "Balancing|Puzzle")
	float TimingTolerance = 1.5f;

	//Leave this value at zero if you want that all buttons have to be pressed in the specified timeframe
	UPROPERTY(EditAnywhere, Category = "Balancing|Puzzle", meta=(ClampMin="0"))
	int MinActivatedTriggers = 0;

	TArray<APuzzleTrigger*> ActivatedPuzzleTriggers;

public:	
	APuzzleObjective();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnPuzzleTriggerTriggered(APuzzleTrigger* TriggeredPuzzleTrigger);

protected:
	void CheckTriggerCount();

	UFUNCTION(BlueprintNativeEvent, Category="PuzzleEvent")
	void OnTriggerNumReached();
	virtual void OnTriggerNumReached_Implementation();

};
