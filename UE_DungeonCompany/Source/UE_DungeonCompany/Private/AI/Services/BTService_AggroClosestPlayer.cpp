// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_AggroClosestPlayer.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "AI/DC_AIController.h"

#include "EngineUtils.h"

UBTService_AggroClosestPlayer::UBTService_AggroClosestPlayer()
{
	bNotifyTick = true;
	NodeName = "AggroClosestPlayer";
}

void UBTService_AggroClosestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ADC_AIController* aiController = Cast<ADC_AIController>(OwnerComp.GetAIOwner());

	if (!aiController)
		return;

	APawn* aiPawn = aiController->GetPawn();

	if (!aiPawn)
		return;

	APlayerCharacter* closestPlayer = nullptr;
	float closestDistance = MaxDistance + 1;

	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		APlayerCharacter* currentPlayer = *It;

		float distance = aiPawn->GetDistanceTo(currentPlayer);

		if (currentPlayer->IsDead() || distance > MaxDistance || distance > closestDistance)
			continue;

		closestDistance = distance;
		closestPlayer = currentPlayer;
	}

	if (closestPlayer)
		aiController->PutAggroOnPlayer(closestPlayer);

}
