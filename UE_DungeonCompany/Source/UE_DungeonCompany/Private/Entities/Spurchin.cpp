// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Spurchin.h"

#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/CharacterMovementComponent.h"

ASpurchin::ASpurchin()
{
	SetActorTickInterval(0.5f);
}

bool ASpurchin::IsInHallway()
{
	FVector start = GetActorLocation();
	FVector rightVector = GetActorRightVector();

	FHitResult rightHit;
	FHitResult leftHit;
	GetWorld()->LineTraceSingleByChannel(rightHit, start, start + rightVector * 1000.f, ECC_GameTraceChannel3);
	DrawDebugLine(GetWorld(), start, start + rightVector * 1000.f, FColor::Blue);
	GetWorld()->LineTraceSingleByChannel(leftHit, start, start - rightVector * 1000.f, ECC_GameTraceChannel3);
	DrawDebugLine(GetWorld(), start, start - rightVector * 1000.f, FColor::Blue);

	if(!rightHit.bBlockingHit || !leftHit.bBlockingHit)
		return false;

	float estimatedWidth = (rightHit.Location - leftHit.Location).Length();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::SanitizeFloat(estimatedWidth));

	return estimatedWidth < MaxHallwayWidth;
}

void ASpurchin::Tick(float DeltaSeconds)
{
	if(IsInHallway())
		GetCharacterMovement()->MaxWalkSpeed = HallwaySpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = OpenSpaceSpeed;

}

void ASpurchin::OnTargetingPlayer_Implementation(APlayerCharacter* Target)
{
	if(!Target)
		return;

	GetSenseConfig<UAISenseConfig_Sight>()->SightRadius = OriginalSightRadius;
	UpdatePerception();
}


void ASpurchin::BeginPlay()
{
	Super::BeginPlay();
	if(!HasAuthority())
		return;

	OriginalSightRadius = GetSenseConfig<UAISenseConfig_Sight>()->SightRadius;
	GetSenseConfig<UAISenseConfig_Sight>()->SightRadius = 0.f;

	UpdatePerception();
}

void ASpurchin::HandleSightSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent)
{
	if (!Stimulus.WasSuccessfullySensed())
	{
		GetSenseConfig<UAISenseConfig_Sight>()->SightRadius = 0.f;
		UpdatePerception();
		Super::HandleSightSense(Actor, Stimulus, BlackboardComponent);
	}
}

void ASpurchin::HandleHearingSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent)
{
	Super::HandleHearingSense(Actor, Stimulus, BlackboardComponent);
}
