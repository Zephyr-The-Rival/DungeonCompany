// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldItem.h"
#include "SendingStone.generated.h"

enum class ESendingStoneAnimatoinState : uint8;

UCLASS()
class UE_DUNGEONCOMPANY_API ASendingStone : public AWorldItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASendingStone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	virtual void TriggerLocalPrimaryAction_Implementation(APlayerCharacter* User) override;
	
	virtual void EndLocalPrimaryActionHold_Implementation(APlayerCharacter* User) override;

public:
	UFUNCTION(BlueprintNativeEvent)
	void Server_SendSignal(ESendingStoneAnimatoinState Signal);
	void Server_SendSignal_Implementation(ESendingStoneAnimatoinState Signal);

public:

	UFUNCTION(BlueprintNativeEvent)
	void ToggleTutorial(bool bOn);
	void ToggleTutorial_Implementation(bool bOn);
};
