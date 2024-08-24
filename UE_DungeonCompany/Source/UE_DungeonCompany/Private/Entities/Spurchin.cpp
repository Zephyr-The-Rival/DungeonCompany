// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Spurchin.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacter/PlayerCharacter.h"

ASpurchin::ASpurchin()
{
	
}	


bool ASpurchin::IsInHallway()
{
	FVector start = GetActorLocation();
	FVector rightVector = GetActorRightVector();

	FHitResult rightHit;
	FHitResult leftHit;
	GetWorld()->LineTraceSingleByChannel(rightHit, start, start + rightVector * 1000.f, ECC_GameTraceChannel3);
	GetWorld()->LineTraceSingleByChannel(leftHit, start, start - rightVector * 1000.f, ECC_GameTraceChannel3);

	if (!rightHit.bBlockingHit || !leftHit.bBlockingHit)
		return false;

	float estimatedWidth = (rightHit.Location - leftHit.Location).Length();

	return estimatedWidth < MaxHallwayWidth;
}

void ASpurchin::OnPlayerAttackHit(APlayerCharacter* PlayerCharacter)
{
	Super::OnPlayerAttackHit(PlayerCharacter);

	if (!IsValid(PlayerCharacter) || !PlayerCharacter->IsDead())
		return;

	AAIController* aiController = GetController<AAIController>();

	if (!aiController)
		return;

	UBlackboardComponent* bbComponent = aiController->GetBlackboardComponent();

	if (!bbComponent)
		return;

	bbComponent->SetValueAsBool(FName("EatingPlayer"), true);
}

void ASpurchin::CheckSpace()
{
	if(IsInHallway())
		GetCharacterMovement()->MaxWalkSpeed = HallwaySpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = OpenSpaceSpeed;
}

void ASpurchin::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void ASpurchin::OnTargetingPlayer_Implementation(APlayerCharacter* Target)
{
	if (!Target)
		return;

	GetSenseConfig<UAISenseConfig_Sight>()->SightRadius = OriginalSightRadius;
	UpdatePerception();
}


void ASpurchin::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(true);
	
	if(!HasAuthority())
		return;

	OriginalSightRadius = GetSenseConfig<UAISenseConfig_Sight>()->SightRadius;
	GetSenseConfig<UAISenseConfig_Sight>()->SightRadius = 0.f;

	UpdatePerception();

	
}
