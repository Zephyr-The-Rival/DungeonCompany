// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimInstances/CatBurglarAnimInstance.h"

#include "Entities/CatBurglar.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCatBurglarAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningCarBurglar = Cast<ACatBurglar>(TryGetPawnOwner());
}

void UCatBurglarAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwningCarBurglar)
	{
		OwningCarBurglar = Cast<ACatBurglar>(TryGetPawnOwner());
		return;
	}

	if (StalkSpeed < 0)
	{
		StalkSpeed = OwningCarBurglar->GetDefaultSpeed();
		RetrieveSpeed = OwningCarBurglar->GetRetrievingSpeed();
		RunSpeed = OwningCarBurglar->GetFleeingSpeed();
	}

	Velocity = OwningCarBurglar->GetVelocity();

	bSteals = OwningCarBurglar->IsStealingItem();
}
