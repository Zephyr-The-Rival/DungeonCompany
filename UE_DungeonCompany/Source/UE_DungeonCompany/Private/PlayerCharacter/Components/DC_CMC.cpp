// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter/Components/DC_CMC.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "WorldActors/Climbable.h"
#include "Items/ClimbingHook/Rope.h"

#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"

bool UDC_CMC::FSavedMove_PlayerCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_PlayerCharacter* newPCMove = static_cast<FSavedMove_PlayerCharacter*>(NewMove.Get());

	if(bWantsToSprint != newPCMove->bWantsToSprint || bWantsToClimb != newPCMove->bWantsToClimb)
		return false;

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UDC_CMC::FSavedMove_PlayerCharacter::Clear()
{
	Super::Clear();

	bWantsToSprint = 0;
	bWantsToClimb = 0;
}

uint8 UDC_CMC::FSavedMove_PlayerCharacter::GetCompressedFlags() const
{
	uint8 result = Super::GetCompressedFlags();

	if(bWantsToSprint)
		result |= FLAG_Custom_0;

	if (bWantsToClimb)
		result |= FLAG_Custom_1;

	return result;
}

void UDC_CMC::FSavedMove_PlayerCharacter::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel,	ClientData);

	UDC_CMC* cmc = Cast<UDC_CMC>(C->GetCharacterMovement());

	bWantsToSprint = cmc->bWantsToSprint;
	bWantsToClimb = cmc->bWantsToClimb;
}

void UDC_CMC::FSavedMove_PlayerCharacter::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	UDC_CMC* cmc = Cast<UDC_CMC>(C->GetCharacterMovement());

	cmc->bWantsToSprint = bWantsToSprint;
	cmc->bWantsToClimb = bWantsToClimb;
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
	bWantsToClimb = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0; 
}

UDC_CMC::UDC_CMC()
{
	SetIsReplicatedByDefault(true);
	NavAgentProps.bCanCrouch = true;
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

void UDC_CMC::PhysClimb(float DeltaTime, int32 Iterations)
{
	if(DeltaTime < MIN_TICK_TIME)
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
	Acceleration = Acceleration.RotateAngleAxis(90.f, ClimbingObject->GetActorRightVector());

	if (Acceleration.Z < 0)
	{
		FHitResult hit;

		FVector start = oldLocation;
		FVector end = start - CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - ClimbingObject->GetActorUpVector() * ClimbingStopHeight;

		GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_GameTraceChannel3);

		if (hit.bBlockingHit)
		{
			bWantsToClimb = false;

			SetMovementMode(MOVE_Falling);
			StartNewPhysics(DeltaTime, Iterations);

			return;
		}
	}

	FVector climbVector = ClimbingObject->GetUpVectorAtZ(oldLocation.Z);
	FVector ladderLocation = ClimbingObject->GetActorLocation() + -UpdatedComponent->GetForwardVector() * ClimbingDistance;

	FHitResult wallHit;
	FHitResult ceilingHit;

	FVector planeNormal = FVector::CrossProduct(climbVector, UpdatedComponent->GetRightVector());
		
	GetWorld()->LineTraceSingleByChannel(wallHit, oldLocation, oldLocation + UpdatedComponent->GetForwardVector() * 50, ECC_GameTraceChannel3);
	GetWorld()->LineTraceSingleByChannel(ceilingHit, oldLocation, oldLocation + ClimbingObject->GetActorUpVector() * 50, ECC_GameTraceChannel3);

	CalcVelocity(DeltaTime, 0.f, false, GetMaxBrakingDeceleration());
	Velocity = FVector::VectorPlaneProject(Velocity, planeNormal);

	FVector delta = DeltaTime * Velocity;
	if (delta.IsNearlyZero())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - oldLocation) / DeltaTime;
		return;
	}
	
	FHitResult hit;
	SafeMoveUpdatedComponent(delta, UpdatedComponent->GetComponentQuat(), true, hit);
	if (ceilingHit.bBlockingHit || hit.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Avoiding ceiling"));
		//FVector avoidanceDelta = ClimbingLadder->GetActorForwardVector() * ClimbingAttractionForce * DeltaTime;
		//SafeMoveUpdatedComponent(avoidanceDelta, UpdatedComponent->GetComponentQuat(), true, hit);
	}
	else if(wallHit.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attracting to wall %s"), *wallHit.Normal.ToString());
		FVector climbingAttractionDelta = -wallHit.Normal * ClimbingAttractionForce * DeltaTime;
		SafeMoveUpdatedComponent(climbingAttractionDelta, UpdatedComponent->GetComponentQuat(), true, hit);
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("correction to ladder position"));
		//FVector correctionDelta = (ladderLocation + ladderVector - GetActorLocation()) * 3 * DeltaTime;
		//SafeMoveUpdatedComponent(correctionDelta, UpdatedComponent->GetComponentQuat(), true, hit);
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
	if (!Super::DoJump(bReplayingMoves))
		return false;

	if (bPrevClimbed)
	{
		Velocity += FVector::UpVector * JumpZVelocity * 0.25f;
		Velocity += -CharacterOwner->GetActorForwardVector() * JumpZVelocity * 0.25f;
		bWantsToClimb = false;
	}

	return true;
}

void UDC_CMC::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (bWantsToClimb && IsValid(ClimbingObject) && !bWantsToCrouch && bCanClimb)
	{
		if (CustomMovementMode != CMOVE_Climb)
		{
			SetMovementMode(MOVE_Custom, CMOVE_Climb);

			bPrevClimbed = true;
			CharacterOwner->bUseControllerRotationYaw = false;

			FRotator newRotation = UpdatedComponent->GetComponentRotation();
			newRotation.Yaw = ClimbingObject->GetClimbRotationYaw();

			float startZ = GetActorLocation().Z;

			FVector upperEnd = ClimbingObject->GetUpperEndLocation();

			if (ClimbingObject->GetUpperEndLocation().Z < startZ)
				startZ = upperEnd.Z;

			FVector forwardVector = FVector::CrossProduct(ClimbingObject->GetUpVectorAtZ(startZ), UpdatedComponent->GetRightVector());

			FVector climbPosition = ClimbingObject->GetLocationAtZ(startZ) + forwardVector * ClimbingDistance;

			FHitResult moveHit;
			SafeMoveUpdatedComponent(climbPosition - GetActorLocation(), newRotation, false, moveHit);
		}
	}
	else if (bPrevClimbed)
	{
		bWantsToClimb = false;
		bPrevClimbed = false;

		SetMovementMode(MOVE_Falling);

		CharacterOwner->bUseControllerRotationYaw = true;
		OnStoppedClimbing.Broadcast();

	}
	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UDC_CMC::ChangeClimbAllowedState(bool IsClimbAllowed)
{
	bCanClimb = IsClimbAllowed;
}

void UDC_CMC::StartSprint()
{
	bWantsToSprint = true;
}

void UDC_CMC::StopSprint()
{
	bWantsToSprint = false;
}

void UDC_CMC::StartClimbing(AClimbable* ActorClimbingAt)
{
	bWantsToClimb = true;
	ClimbingObject = ActorClimbingAt;

	if(CharacterOwner->HasAuthority())
		Multicast_SetClimbingObject(ActorClimbingAt);
	else
		Server_SetClimbingObject(ActorClimbingAt);

	FRotator newRotation = UKismetMathLibrary::MakeRotFromZX(FVector::UpVector, -ActorClimbingAt->GetActorForwardVector());	

	CharacterOwner->GetController()->SetControlRotation(newRotation);

}

void UDC_CMC::StopClimbing()
{
	bWantsToClimb = false;
}

void UDC_CMC::Server_SetClimbingObject_Implementation(AClimbable* InClimbingObject)
{
	Multicast_SetClimbingObject(InClimbingObject);
}

void UDC_CMC::Multicast_SetClimbingObject_Implementation(AClimbable* InClimbingObject)
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
