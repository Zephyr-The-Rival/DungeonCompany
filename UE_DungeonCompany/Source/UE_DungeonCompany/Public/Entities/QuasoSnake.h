// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/AIEntity.h"
#include "QuasoSnake.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API AQuasoSnake : public AAIEntity
{
	GENERATED_BODY()

public:
	virtual void AttackPlayer(APlayerCharacter* TargetPlayer) override;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_OnAttachedToPlayer();
	void Multicast_OnAttachedToPlayer_Implementation();
	
};
