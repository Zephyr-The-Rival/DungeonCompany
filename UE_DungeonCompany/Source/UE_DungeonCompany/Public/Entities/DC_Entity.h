// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DC_Entity.generated.h"

class UBehaviorTree;

UCLASS()
class UE_DUNGEONCOMPANY_API ADC_Entity : public ACharacter
{
	GENERATED_BODY()

public:
	ADC_Entity();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;
public:
	inline UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

};
