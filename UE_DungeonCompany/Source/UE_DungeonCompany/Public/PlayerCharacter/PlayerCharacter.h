// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/DC_Entity.h"
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
class UE_DUNGEONCOMPANY_API APlayerCharacter : public ADC_Entity
{
	GENERATED_BODY()

private: 
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = "Camera", BlueprintReadOnly)
	UCameraComponent* FirstPersonCamera;

	TObjectPtr<USkeletalMeshComponent> GetFirstPersonMesh() const { return this->FirstPersonMesh; }

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void LocalTick(float DeltaTime);
	virtual void StaminaTick(float DeltaTime);

private:
	UPROPERTY(EditAnywhere, Category = "Input | Mapping")
	UInputMappingContext* CharacterInputMapping;

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
	UInputAction* ItemPrimaryAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* ItemSecondaryAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* DropItemAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* SwitchHandAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* InventoryAction;

	//Inventory Input

	UPROPERTY(EditAnywhere, Category = "Input | Mapping")
	UInputMappingContext* InventoryInputMapping;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* NavigateInventoryAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* EquipItemInvAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* DropItemInvAction;
	
public:
	inline UInputMappingContext* GetCharacterInputMapping() const { return CharacterInputMapping; }
	inline UInputMappingContext* GetInventoryInputMapping() const { return InventoryInputMapping; }

	class UEnhancedInputLocalPlayerSubsystem* GetInputLocalPlayer() const;

	void ActivateCharacterInputMappings();
	void DeactivateCharacterInputMappings();

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
	float InteractionRange=200;

	void InteractorLineTrace();

	void DestroyWorldItem(AWorldItem* ItemToDestroy);

	UFUNCTION(Server, Unreliable)
	void Server_DestroyWorldItem(AWorldItem* ItemToDestroy);
	void Server_DestroyWorldItem_Implementation(AWorldItem* ItemToDestroy);

public:
	void Interact();

protected:
	UFUNCTION(Server, Unreliable)
	void Server_Interact(UObject* Interactable);
	void Server_Interact_Implementation(UObject* Interactable);

public:
	void PickUpItem(AWorldItem* WorldItem);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Movement")
	float WalkingSpeed = 350;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Movement")
	float CrouchedWalkingSpeed = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Movement")
	float SprintSpeedMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Movement")
	float ClimbingSpeed = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Movement")
	float JumpVelocity = 420.f;

private:
	UPROPERTY(BlueprintGetter= GetIsSprinting)
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

	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	bool GetIsSprinting() const {return this->bSprinting;}

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
	float JumpStaminaDrain = 1.f;

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

public:
	void SetVoiceEffect(USoundEffectSourcePresetChain* SoundEffect);

protected:
	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;

private:
	class UAIPerceptionStimuliSourceComponent* StimulusSource;

public:
	void DeactivateStimulus();

protected://inventory & Backpack
	UPROPERTY(EditAnywhere, BlueprintGetter = GetInventory)
	UInventory* Inventory;

	UPROPERTY(EditAnywhere, BlueprintGetter = GetBackpack)
	UInventory* Backpack;

	bool bSlotAIsInHand = true;

	bool bInventoryIsOn = false;

	

protected:
	void ToggleInventory();

public:
	UInventorySlot* GetCurrentlyHeldInventorySlot();

private:
	UInventorySlot* FindFreeSlot();

	UPROPERTY(Replicated)
	AWorldItem* CurrentlyHeldWorldItem;

protected:
	void TakeOutItem();

	UFUNCTION(Server, Unreliable)
	void Server_SetTPMeshAnimClass(UClass* NewClass);
	void Server_SetTPMeshAnimClass_Implementation(UClass* NewClass);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SetTPMeshAnimClass(UClass* NewClass);
	void Multicast_SetTPMeshAnimClass_Implementation(UClass* NewClass);

	void SpawnItemInHand(TSubclassOf<AWorldItem> ItemToSpawn, const FString& SerializedData);

	UFUNCTION(Server, Unreliable)
	void Server_SpawnItemInHand(TSubclassOf<AWorldItem> ItemToSpawn, const FString& SerializedData);
	void Server_SpawnItemInHand_Implementation(TSubclassOf<AWorldItem> ItemToSpawn, const FString& SerializedData);

	UFUNCTION(Server, Unreliable)
	void AddMoneyToWallet(float Amount);
	void AddMoneyToWallet_Implementation(float Amount);

	void DropItem(UInventorySlot* SlotToEmpty);

	void RemoveInventorySlot(UInventorySlot* SlotToEmpty);

	void SwitchHand();

	UFUNCTION()
	void AllowSwitchHand();

	void EquipCurrentInventorySelection(bool BToA);

	void DropItemPressed();

protected:
	void NavigateInventory(const FInputActionValue& Value);

	void EquipItem(const FInputActionValue& Value);

	void DropItemInvPressed();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UObject> NoItemFirstPersonAnimationBlueprintClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UObject> NoItemThirdPersonAnimationBlueprintClass;

	void TriggerPrimaryItemAction();

	UFUNCTION(Server, Unreliable)
	void Server_TriggerPrimaryItemAction();

	void TriggerSecondaryItemAction();

	UFUNCTION(Server, Unreliable)
	void Server_TriggerSecondaryItemAction();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bHasBackPack = false;

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
	void SpawnDroppedWorldItem(TSubclassOf<AWorldItem> ItemToSpawn, const FString& SerializedData);
	UFUNCTION(Server,Unreliable)
	void Server_SpawnDroppedWorldItem(TSubclassOf<AWorldItem> ItemToSpawn, const FString& SerializedData);
	void Server_SpawnDroppedWorldItem_Implementation(TSubclassOf<AWorldItem> ItemToSpawn, const FString& SerializedData);

public:
	void ReportTalking(float Loudness);

private:
	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* CoughSound;

public:
	void Cough();

private:
	float LastStandingHeight;
	bool BWasFallingInLastFrame = false;

protected:
	void CheckForFallDamage();
	float FallDamageCalculation(float deltaHeight);

public://blockers

	bool bSwitchHandAllowed = true;
	bool bMoveAllowed = true;
	bool bLookAllowed = true;
	bool bSprintAllowed = true;
	bool bPrimaryActionAllowed = true;
	bool bSecondaryActionAllowed = true;

public://fighting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float AttackBlend = 0;

	void StartAttacking();

	void AttackStart();

	UFUNCTION(Server, Unreliable)
	void Server_AttackStart();
	void Server_AttackStart_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_AttackStart();
	void Multicast_AttackStart_Implementation();

	UFUNCTION(BlueprintCallable)
	void AttackLanded();

	UFUNCTION(BlueprintCallable)
	void OnAttackOver();

public:
	virtual void OnDeath_Implementation() override;

};
