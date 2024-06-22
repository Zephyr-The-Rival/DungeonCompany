// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DC_CMC.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None		UMETA(Hidden),
	CMOVE_Climb		UMETA(DisplayName = "Climb")
};

UCLASS()
class UE_DUNGEONCOMPANY_API UDC_CMC : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FSavedMove_PlayerCharacter : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;

		uint8 bWantsToSprint:1;

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	class FNetworkPredictionData_Client_PlayerCharacter : public FNetworkPredictionData_Client_Character
	{
			typedef FNetworkPredictionData_Client_Character Super;

		public:
			FNetworkPredictionData_Client_PlayerCharacter(const UCharacterMovementComponent& ClientMovement);

			virtual FSavedMovePtr AllocateNewMove() override;
	};

public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

private:
	bool bWantsToSprint = false;

	UPROPERTY(EditDefaultsOnly)
	float MaxSprintSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxClimbSpeed = 300.f;

	UPROPERTY(EditDefaultsOnly)
	float BrakingDecelerationClimbing = 1000.f;

	UPROPERTY(EditDefaultsOnly)
	float ClimbingDistance = 100.f;

	UPROPERTY(EditDefaultsOnly)
	float ClimbingAttractionForce = 600.f;

private:
	UPROPERTY(Transient)
	AActor* ClimbingObject;

protected:
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

private:
	void PhysClimb(float DeltaTime, int32 Iterations);

public:
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;

	virtual bool CanAttemptJump() const override;
	virtual bool DoJump(bool bReplayingMoves) override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

public:
	void StartSprint();
	void StopSprint();

	void StartClimbing(AActor* ActorClimbingAt);

	UFUNCTION(BlueprintCallable)
	bool IsCustomMovementModeActive(ECustomMovementMode InCustomMovementMode) const;

};
