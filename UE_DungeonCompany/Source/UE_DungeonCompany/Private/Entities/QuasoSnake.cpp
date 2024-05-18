// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/QuasoSnake.h"

#include "PlayerCharacter/PlayerCharacter.h"
#include "AI/DC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void AQuasoSnake::AttackPlayer(APlayerCharacter* TargetPlayer)
{
	Super::AttackPlayer(TargetPlayer);
	Multicast_OnAttachedToPlayer();
	AttachToActor(TargetPlayer, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SetActorLocation(TargetPlayer->GetActorLocation() + FVector::UpVector * 100);
	Cast<ADC_AIController>(GetController())->GetBlackboardComponent()->SetValueAsObject("PlayerAttachedTo", TargetPlayer);

}

void AQuasoSnake::Multicast_OnAttachedToPlayer_Implementation()
{
	GetCapsuleComponent()->SetCollisionProfileName("OverlapAll", true);
	GetCharacterMovement()->Deactivate();
}
