// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Interactable.h"
#include "PlayerCharacter.generated.h"

class UVOIPTalker;

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

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Movement")
	float WalkingSpeed = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Movement")
	float SprintSpeedMultiplier = 1.5f;

private:
	bool bSprinting = false;

protected:

	void MoveRight(float Value);
	void MoveForward(float Value);

	void Move(FVector MoveVector);

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
