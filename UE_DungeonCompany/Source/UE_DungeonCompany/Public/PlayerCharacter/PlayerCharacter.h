// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Interactable.h"
#include "PlayerCharacter.generated.h"

class UVOIPTalker;
class AWorldItem;

UCLASS()
class UE_DUNGEONCOMPANY_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

private: 
	UPROPERTY(EditAnywhere, Category = "Camera")
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void LocalTick(float DeltaTime);
	virtual void StaminaTick(float DeltaTime);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	IInteractable* CurrentInteractable;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing")
	float InteractionRange=170;

	void InteractorLineTrace();

		UFUNCTION(SERVER, Unreliable)
	void DestroyWorldItem(AWorldItem* ItemToDestroy);

public:
	void Interact();

	void PickUpItem(AWorldItem* WorldItem);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Movement")
	float WalkingSpeed = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Movement")
	float SprintSpeedMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Movement")
	float ClimbingSpeed = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Movement")
	float JumpVelocity = 420.f;

private:
	bool bSprinting = false;
	
protected:
	void MoveRight(float Value);
	void MoveForward(float Value);
	void Move(FVector MoveVector);
	
	virtual void Jump() override;

	void ToggleCrouch();
	void ToggleSprint();

	void StartSprint();

	UFUNCTION(Server, Unreliable)
	void Server_StartSprint();
	void Server_StartSprint_Implementation();

	void StopSprint();

	UFUNCTION(Server, Unreliable)
	void Server_StopSprint();
	void Server_StopSprint_Implementation();

public:
	UFUNCTION(Server, Unreliable)
	void Server_SetActorLocation(const FVector& InLocation);
	void Server_SetActorLocation_Implementation(const FVector& InLocation);

	UFUNCTION(Server, Unreliable)
	void Server_LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride);
	void Server_LaunchCharacter_Implementation(FVector LaunchVelocity, bool bXYOverride, bool bZOverride);

private:
	bool bClimbing = false;

public:
	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStoppedClimbing);

	FOnStoppedClimbing OnStoppedClimbing;

	void StartClimbingAtLocation(const FVector& Location);
	void StopClimbing();

protected:

	UFUNCTION(Server, Unreliable)
	void Server_StartClimbingAtLocation(const FVector& Location);
	void Server_StartClimbingAtLocation_Implementation(const FVector& Location);

	UFUNCTION(Server, Unreliable)
	void Server_StopClimbing();
	void Server_StopClimbing_Implementation();

public:
	virtual bool CanJumpInternal_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Stamina")
	float MaxStamina = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Stamina")
	float SprintStaminaDrainPerSecond = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Stamina")
	float StaminaGainPerSecond = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Stamina")
	float StaminaGainDelay = 3.f;

private:
	UPROPERTY(BlueprintGetter=GetStamina)
	float Stamina = MaxStamina;
	bool bResting = false;

	FTimerHandle RestDelayTimerHandle;
	FTimerDelegate RestDelegate;

public:
	void AddStamina(float AddingStamina);
	void SubstractStamina(float SubStamina);
	
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	float GetStamina() const { return Stamina; }
private:
	UVOIPTalker* VOIPTalker;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* VoiceSA;

protected:
	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;

};
