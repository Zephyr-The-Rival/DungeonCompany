// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_SetBoolAtTick.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UBTService_SetBoolAtTick : public UBTService_BlackboardBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	bool NewValue = false;

public:
	UBTService_SetBoolAtTick();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
