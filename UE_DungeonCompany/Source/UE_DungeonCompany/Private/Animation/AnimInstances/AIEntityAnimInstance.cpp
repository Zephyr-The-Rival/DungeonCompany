
// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimInstances/AIEntityAnimInstance.h"
#include "Entities/AIEntity.h"

void UAIEntityAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningEntity = Cast<AAIEntity>(TryGetPawnOwner());

	if(!OwningEntity)
		return;

	LastHP = OwningEntity->GetHealth();

}

void UAIEntityAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwningEntity)
	{
		OwningEntity = Cast<AAIEntity>(TryGetPawnOwner());
		return;
	}

	bAttacking = OwningEntity->IsAttacking();
	bDead = OwningEntity->IsDead();

	bTookDamage = OwningEntity->GetHealth() < LastHP;

	LastHP = OwningEntity->GetHealth();
}
