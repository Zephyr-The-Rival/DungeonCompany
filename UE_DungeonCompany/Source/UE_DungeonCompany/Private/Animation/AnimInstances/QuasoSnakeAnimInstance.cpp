// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimInstances/QuasoSnakeAnimInstance.h"
#include "Entities/QuasoSnake.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

void UQuasoSnakeAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningQuaso = Cast<AQuasoSnake>(TryGetPawnOwner());
}

void UQuasoSnakeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwningQuaso)
	{
		OwningQuaso = Cast<AQuasoSnake>(TryGetPawnOwner());
		return;
	}

	Velocity = OwningQuaso->GetVelocity() * (0.f < OwningQuaso->GetCharacterMovement()->GetCurrentAcceleration().
	                                                            Length());
	Location = OwningQuaso->GetActorLocation();

	TargetEyeRotation = !IsValid(OwningQuaso->GetTargetPlayer())
		              ? FRotator::ZeroRotator
		              : (OwningQuaso->GetTargetPlayer()->GetActorLocation() - GetSkelMeshComponent()->GetBoneLocation(
			              FName("DEF_BODY_007"))).Rotation();

	bAttachedToPlayer = OwningQuaso->IsAttachedToPlayer();
	bAttacking = bAttacking || bAttachedToPlayer;
	bLurking = OwningQuaso->IsLurking();

	APlayerCharacter* playerAttachedTo = OwningQuaso->GetPlayerAttachedTo();

	bAttachedFirstPersonPlayer = !bAttachedToPlayer
		                             ? false
		                             : !IsValid(playerAttachedTo)
		                             ? false
		                             : playerAttachedTo->IsLocallyControlled();
}
