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
	APuzzleTrigger();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	uint8 GetChannel() const { return Channel; }

public:
	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggered, APuzzleTrigger*, PuzzleTrigger);

	FOnTriggered OnTriggered;

	void Trigger();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_Triggered();
	void Multicast_Triggered_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category="PuzzleEvent")
	void OnTriggerStarted();
	virtual void OnTriggerStarted_Implementation();


	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_NotTriggered();
	void Multicast_NotTriggered_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category="PuzzleEvent")
	void OnTriggerStopped();
	virtual void OnTriggerStopped_Implementation();

};
