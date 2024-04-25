// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Interactable.h"
#include "PlayerCharacter.generated.h"

class AWorldItem;

class UVOIPTalker;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

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

private:
	UPROPERTY(EditAnywhere, Category = "Input | Mapping")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* InteractAction;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	IInteractable* CurrentInteractable;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing")
	float InteractionRange=170;

	void InteractorLineTrace();

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
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
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
	float Stamina = MaxStamina;
	bool bResting = false;

	FTimerHandle RestDelayTimerHandle;
	FTimerDelegate RestDelegate;

public:
	void AddStamina(float AddingStamina);
	void SubstractStamina(float SubStamina);

private:
	UVOIPTalker* VOIPTalker;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* VoiceSA;

protected:
	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;

};
