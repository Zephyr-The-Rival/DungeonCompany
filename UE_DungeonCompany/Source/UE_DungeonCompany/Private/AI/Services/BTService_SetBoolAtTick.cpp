// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_SetBoolAtTick.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTService_SetBoolAtTick::UBTService_SetBoolAtTick()
{
	bNotifyTick = true;
	NodeName = "Set Bool At Tick";
	bRestartTimerOnEachActivation = true;
}

void UBTService_SetBoolAtTick::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), NewValue);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::FromInt(NewValue));

}
