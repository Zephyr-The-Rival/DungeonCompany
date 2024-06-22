// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter/Components/DC_CMC.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "WorldActors/Ladder.h"
#include "Items/ClimbingHook/Rope.h"

#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"

bool UDC_CMC::FSavedMove_PlayerCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_PlayerCharacter* newPCMove = static_cast<FSavedMove_PlayerCharacter*>(NewMove.Get());

	if(bWantsToSprint != newPCMove->bWantsToSprint)
		return false;

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UDC_CMC::FSavedMove_PlayerCharacter::Clear()
{
	Super::Clear();

	bWantsToSprint = 0;
}

uint8 UDC_CMC::FSavedMove_PlayerCharacter::GetCompressedFlags() const
{
	uint8 result = Super::GetCompressedFlags();

	if(bWantsToSprint)
		result |= FLAG_Custom_0;

	return result;
}

void UDC_CMC::FSavedMove_PlayerCharacter::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel,	ClientData);

	UDC_CMC* cmc = Cast<UDC_CMC>(C->GetCharacterMovement());

	bWantsToSprint = cmc->bWantsToSprint;
}

void UDC_CMC::FSavedMove_PlayerCharacter::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	UDC_CMC* cmc = Cast<UDC_CMC>(C->GetCharacterMovement());

	cmc->bWantsToSprint = bWantsToSprint;
}

FNetworkPredictionData_Client* UDC_CMC::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr)

	if (ClientPredictionData == nullptr)
	{
		UDC_CMC* mutableThis = const_cast<UDC_CMC*>(this);
		mutableThis->ClientPredictionData = new FNetworkPredictionData_Client_PlayerCharacter(*this);
		mutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		mutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return Super::GetPredictionData_Client();
}

void UDC_CMC::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

UDC_CMC::UDC_CMC()
{
	SetIsReplicatedByDefault(true);
}

void UDC_CMC::PhysCustom(float DeltaTime, int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_Climb:
		PhysClimb(DeltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"));
	}
}

void UDC_CMC::OnClimbVolumeLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor != CharacterOwner)
		return;

	SetMovementMode(MOVE_Falling);
}

void UDC_CMC::PhysClimb(float DeltaTime, int32 Iterations)
{
	if (!IsValid(ClimbingObject) && GetOwner()->HasAuthority())
	{
		SetMovementMode(MOVE_Falling);
		StartNewPhysics(DeltaTime, Iterations);
		return;

	}

	if(DeltaTime < MIN_TICK_TIME)
		return;

	if(!IsValid(ClimbingObject))
		return;

	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	bJustTeleported = false;
	Iterations++;
	const FVector oldLocation = UpdatedComponent->GetComponentLocation();
	
	Acceleration.Z = 0.f;
	Acceleration.X = 0.f;
	Acceleration = Acceleration.RotateAngleAxis(90.f, ClimbingObject->GetActorRightVector());

	CalcVelocity(DeltaTime, 0.f, false, GetMaxBrakingDeceleration());
	Velocity = FVector::VectorPlaneProject(Velocity, ClimbingObject->GetActorForwardVector());

	const FVector delta = DeltaTime * Velocity;
	if (!delta.IsNearlyZero())
	{
		FHitResult hit;
		SafeMoveUpdatedComponent(delta, UpdatedComponent->GetComponentQuat(), true, hit);
		//FVector climbingAttractionDelta = -ClimbingObject->GetActorForwardVector() * ClimbingAttractionForce * DeltaTime;
		//SafeMoveUpdatedComponent(climbingAttractionDelta, UpdatedComponent->GetComponentQuat(), true, hit);
	}

	Velocity = (UpdatedComponent->GetComponentLocation() - oldLocation) / DeltaTime;
}

float UDC_CMC::GetMaxSpeed() const
{
	if (MovementMode == MOVE_Walking && bWantsToSprint && !IsCrouching())
		return MaxSprintSpeed;

	if (MovementMode != MOVE_Custom) 
		return Super::GetMaxSpeed();

	switch (CustomMovementMode)
	{
	case CMOVE_Climb:
		return MaxClimbSpeed;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"));
		return -1.f;
	}
}

float UDC_CMC::GetMaxBrakingDeceleration() const
{
	if (MovementMode != MOVE_Custom)
		return Super::GetMaxBrakingDeceleration();

	switch (CustomMovementMode)
	{
	case CMOVE_Climb:
		return BrakingDecelerationClimbing;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"));
		return -1.f;
	}
}

bool UDC_CMC::CanAttemptJump() const
{
	return Super::CanAttemptJump() || IsCustomMovementModeActive(CMOVE_Climb);

}

bool UDC_CMC::DoJump(bool bReplayingMoves)
{
	bool bWasClimbing = IsCustomMovementModeActive(CMOVE_Climb);
	if (!Super::DoJump(bReplayingMoves))
		return false;

	if (bWasClimbing)
	{
		Velocity += FVector::UpVector * JumpZVelocity * 0.25f;
		Velocity += -CharacterOwner->GetActorForwardVector() * JumpZVelocity * 0.25f;
		OnStoppedClimbing.Broadcast();
	}

	return true;
}

void UDC_CMC::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UDC_CMC::StartSprint()
{
	bWantsToSprint = true;
}

void UDC_CMC::StopSprint()
{
	bWantsToSprint = false;
}

void UDC_CMC::StartClimbing(AActor* ActorClimbingAt)
{
	if(!CharacterOwner->HasAuthority())
		return;

	ALadder* ladder = Cast<ALadder>(ActorClimbingAt);
	if (!IsValid(ladder))
		return;

	ladder->GetClimbVolume()->OnComponentEndOverlap.AddDynamic(this, &UDC_CMC::OnClimbVolumeLeft);

	ClimbingObject = ladder;
	Multicast_SetClimbingObject(ladder);

	FQuat newRotation = UKismetMathLibrary::MakeRotFromZY(ActorClimbingAt->GetActorUpVector(), -ActorClimbingAt->GetActorForwardVector()).Quaternion();

	float distanceToStart = CharacterOwner->GetDistanceTo(ActorClimbingAt);

	if (distanceToStart > ladder->GetHeight())
		distanceToStart = ladder->GetHeight();

	FVector climbPosition = ActorClimbingAt->GetActorLocation() + ActorClimbingAt->GetActorUpVector() * distanceToStart + ActorClimbingAt->GetActorForwardVector() * ClimbingDistance;

	FHitResult moveHit;
	SafeMoveUpdatedComponent(climbPosition-GetActorLocation(), newRotation, false, moveHit);
	SetMovementMode(MOVE_Custom, CMOVE_Climb);

	bUseControllerDesiredRotation = false;
}

void UDC_CMC::StopClimbing()
{
	SetMovementMode(MOVE_Falling);
}

void UDC_CMC::Multicast_SetClimbingObject_Implementation(AActor* InClimbingObject)
{
	ClimbingObject = InClimbingObject;
}

bool UDC_CMC::IsCustomMovementModeActive(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

UDC_CMC::FNetworkPredictionData_Client_PlayerCharacter::FNetworkPredictionData_Client_PlayerCharacter(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr UDC_CMC::FNetworkPredictionData_Client_PlayerCharacter::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_PlayerCharacter());
}
