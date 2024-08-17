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
	float Range = 100.f;

	UPROPERTY(EditAnywhere)
	bool bIgnoreZValue = false;

	UPROPERTY(EditAnywhere, meta = (EditCondition="!bCheckAgainstActorClass"))
	FBlackboardKeySelector CheckingActor;

	UPROPERTY(EditAnywhere, meta = (EditCondition="bCheckAgainstActorClass"))
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere)
	bool bCheckAgainstActorClass = false;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector WriteResultTo;

public:
	UBTService_IsInRange();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool IsInRangeOfActorType(const AActor* InActor) const;
};
