// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimInstances/QuasoSnakeAnimInstance.h"
#include "Entities/QuasoSnake.h"

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

	Velocity = OwningQuaso->GetVelocity() * (0.f < OwningQuaso->GetCharacterMovement()->GetCurrentAcceleration().Length());
	Location = OwningQuaso->GetActorLocation();

	bAttachedToPlayer = OwningQuaso->IsAttachedToPlayer();
	bLurking = OwningQuaso->IsLurking();

	FlushDebugStrings(GetWorld());
	FString debugString = "bIsAttacking = " + FString::FromInt(bAttacking) + 
							"\n bHit = " + FString::FromInt(bHit) + 
							"\n bDead = " + FString::FromInt(bDead) +
							"\n bLurking = " + FString::FromInt(bLurking);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, debugString);

}
