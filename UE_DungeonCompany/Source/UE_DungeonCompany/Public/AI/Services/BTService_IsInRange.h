// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_IsInRange.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UBTService_IsInRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	float AttackRange = 100.f;

	UPROPERTY(EditAnywhere)
	bool bIgnoreZValue = false;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector CheckingActor;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector WriteResultTo;

public:
	UBTService_IsInRange();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
