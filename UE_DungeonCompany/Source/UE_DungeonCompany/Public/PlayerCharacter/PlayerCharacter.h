// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Interactable.h"
#include "PlayerCharacter.generated.h"

class AWorldItem;
class UItemData;
class UVOIPTalker;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UInventory;
class UInventorySlot;


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

	UPROPERTY(EditAnywhere, Getter = IsCrouchOnHold, Category = "Input | Toggle/Hold")
	bool bCrouchHold = false;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, Getter = IsSprintOnHold, Category = "Input | Toggle/Hold")
	bool bSprintHold = false;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* DPadUpAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* DPadDownAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* DPadLeftAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* DPadRightAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* ToggleInventoryPCAction;
	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* ToggleInventoryControllerAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* FaceUpAction;
	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* FaceDownAction;
	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* FaceLeftAction;
	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* FaceRightAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* MouseRightAction;
	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* MouseLeftAction;

public:
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	bool IsCrouchOnHold() const;

	UFUNCTION(BlueprintCallable)
	void SetCrouchHold(bool ShouldHoldCrouch);

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	bool IsSprintOnHold() const;

	UFUNCTION(BlueprintCallable)
	void SetSprintHold(bool ShouldHoldSprint);

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private://interact
	IInteractable* CurrentInteractable;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing")
	float InteractionRange=170;

	void InteractorLineTrace();

	void DestroyWorldItem(AWorldItem* ItemToDestroy);
	UFUNCTION(Server, Unreliable)
	void Server_DestroyWorldItem(AWorldItem* ItemToDestroy);
	void Server_DestroyWorldItem_Implementation(AWorldItem* ItemToDestroy);

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
	void NoMove();
	void Look(const FInputActionValue& Value);
	
	virtual void Jump() override;

	void CrouchActionStarted();
	void CrouchActionCompleted();

	void ToggleCrouch();

	void SprintActionStarted();
	void SprintActionCompleted();

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
	FVector ClimbUpVector = FVector::UpVector;

public:
	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStoppedClimbing);

	FOnStoppedClimbing OnStoppedClimbing;

	void StartClimbingAtLocation(const FVector& Location, const FVector& InClimbUpVector);
	void StopClimbing();

protected:

	UFUNCTION(Server, Unreliable)
	void Server_StartClimbingAtLocation(const FVector& Location, const FVector& InClimbUpVector);
	void Server_StartClimbingAtLocation_Implementation(const FVector& Location, const FVector& InClimbUpVector);

	UFUNCTION(Server, Unreliable)
	void Server_StopClimbing();
	void Server_StopClimbing_Implementation();

public:
	virtual bool CanJumpInternal_Implementation() const override;

protected:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Stamina")
	float SprintStaminaDrainPerSecond = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Stamina")
	float StaminaGainPerSecond = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Stamina")
	float StaminaGainDelay = 3.f;

private:
	UPROPERTY(BlueprintGetter=GetStamina)
	float Stamina;
	bool bResting = false;

	FTimerHandle RestDelayTimerHandle;
	FTimerDelegate RestDelegate;

public:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Stamina")
	float MaxStamina = 5.f;

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


protected://inventory & Backpack


		UPROPERTY(EditAnywhere, BlueprintGetter= GetInventory)
		UInventory* Inventory;

		UPROPERTY(EditAnywhere, BlueprintGetter= GetBackpack)
		UInventory* Backpack;

		bool BSlotAIsInHand=true;

		void ToggleInventoryPC();
		void ToggleInventoryController();
		void ToggleInventory(bool ControllerVersion);
		bool BInventoryIsOn=false;

		UInventorySlot* GetCurrentlyHeldInventorySlot();
		UInventorySlot* FindFreeSlot();

		void TakeOutItem();
		AWorldItem* CurrentlyHeldWorldItem;

		void SpawnItemInHand(TSubclassOf<AWorldItem> ItemToSpawn);
		UFUNCTION(Server, Unreliable)
		void Server_SpawnItemInHand(TSubclassOf<AWorldItem> ItemToSpawn);
		void Server_SpawnItemInHand_Implementation(TSubclassOf<AWorldItem> ItemToSpawn);

		void DropItem(UInventorySlot* SlotToEmpty);
		
		void SwitchHand();
		bool BSwichHandAllowed=true;
		UFUNCTION()
		void AllowSwitchHand();

		void EquipCurrentInventorySelection(bool BToA);


public:
		UPROPERTY(EditAnywhere,BlueprintReadOnly)
		bool BHasBackPack = false;

		UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
		UInventory* GetInventory() const { return Inventory; }

		UFUNCTION(BlueprintPure,BlueprintInternalUseOnly)
		UInventory* GetBackpack() const {return Backpack;}

		UPROPERTY(EditAnywhere,BlueprintReadOnly)
		UInventorySlot* HandSlotA;
		UPROPERTY(EditAnywhere,BlueprintReadOnly)
		UInventorySlot* HandSlotB;
		//bool BItemAIsInHand is protected
		


private:
	class UAIPerceptionStimuliSourceComponent* StimulusSource;

private:

	void SpawnDroppedWorldItem(TSubclassOf<AWorldItem> ItemToSpawn);
	UFUNCTION(Server,Unreliable)
	void Server_SpawnDroppedWorldItem(TSubclassOf<AWorldItem> ItemToSpawn);
	void Server_SpawnDroppedWorldItem_Implementation(TSubclassOf<AWorldItem> ItemToSpawn);

public:
	void ReportTalking(float Loudness);
		
public://Health

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	float GetHealth() const { return this->HP; }
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Balancing")
	float MaxHP=100;

protected:


private:
	UPROPERTY(EditAnywhere,BlueprintGetter=GetHealth)
	float HP;

	void TakeDamage_DC(float amout);

	void CheckForFallDamage();
	float LastStandingHeight;
	bool BWasFallingInLastFrame=false;

private://only controller controls

	void DPadUpPressed();
	void DPadDownPressed();
	void DPadLeftPressed();
	void DPadRightPressed();
	
	void FaceUpPressed();
	void FaceDownPressed();
	void FaceLeftPressed();
	void FaceRightPressed();

	//only pc controls
	void LeftMouseButtonPressed();
	void RightMouseButtonPressed();

};
