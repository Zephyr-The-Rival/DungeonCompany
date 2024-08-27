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

	Velocity = OwningQuaso->GetVelocity();
	Location = OwningQuaso->GetActorLocation();
	
	FVector worldTargetEyeVector = !IsValid(OwningQuaso->GetTargetPlayer())
					  ? OwningQuaso->GetActorLocation() + OwningQuaso->GetActorForwardVector()
					  : OwningQuaso->GetTargetPlayer()->GetMesh()->GetBoneLocation(FName("DEF_Head_001")) - GetSkelMeshComponent()->GetBoneLocation(
						  FName("DEF_JOINT_008"));

	TargetEyeRotation = worldTargetEyeVector.Rotation() - OwningQuaso->GetActorRotation() ;
	
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
