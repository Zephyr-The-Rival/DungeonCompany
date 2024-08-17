// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/DC_Entity.h"
#include "Camera/CameraComponent.h"
#include "InputFunctionLibrary.h"
#include "Interactable.h"
#include "Entities/FootstepSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PlayerCharacter.generated.h"

class AWorldItem;
class UItemData;
class UVOIPTalker;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
struct FSlotData;
class UInventory;
class UInventorySlot;
class ABackPack;
class ABuyableItem;
class UPlayerHud;
class AItemSocket;
class AWeapon;
class UFootstepSystemComponent;

struct  FWeaponInfo;

USTRUCT()
struct FHeldItem
{
	GENERATED_BODY()
	TSubclassOf<UItemData> ItemDataClass;
	FString ItemData;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemDrop);

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DropTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing")
	float throwStrengh=800;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void LocalTick(float DeltaTime);
	virtual void StaminaTick(float DeltaTime);

	virtual void Restart() override;

protected:
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
	UInputAction* ThrowItemAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* SwitchHandAction;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* InventoryAction;

	//Inventory Input

	UPROPERTY(EditAnywhere, Category = "Input | Mapping")
	UInputMappingContext* InventoryInputMapping;

	UPROPERTY(EditAnywhere, Category = "Input | Action")
	UInputAction* ToggleInventoryInvAction;

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
	void ResetInteractPrompt();

protected:

	void InteractPressed();
	void StartHoldInteract();
	void StopHoldInteract();

	void CheckHoldInteract();
	bool bIsHoldingInteract=false;
	float InteractHoldingSecondCounter=0;

public:

	UFUNCTION(Blueprintable,BlueprintPure)
	float GetInteractHoldingSecondsCounter() const {return this->InteractHoldingSecondCounter;}
	
protected:
	
	UFUNCTION(Server, Unreliable)
	void Server_Interact(UObject* Interactable);
	void Server_Interact_Implementation(UObject* Interactable);

public:
	void PickUpItem(AWorldItem* WorldItem);

protected:
	void PickUpBackpack(ABackPack* BackpackToPickUp);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Balancing/Movement")
	float GamepadAccelerationSpeed = 7.f;

private:
	UPROPERTY(BlueprintGetter= GetIsSprinting)
	bool bSprinting = false;
	
private:
	void (*LookFunction)(const FInputActionValue& Value, APawn* Player) = &UInputFunctionLibrary::LookGamepad;

protected:
	void Move(const FInputActionValue& Value);
	void NoMove();

	void Look(const FInputActionValue& Value);

	void NoLook();
	
	virtual void Jump() override;

	void CrouchActionStarted();
	void CrouchActionCompleted();

	void ToggleCrouch();

	void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Sounds")
	USoundBase* CrouchSound;

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

	inline FVector GetBaseAimDirection() const { return GetBaseAimRotation().Vector(); }

protected:
	UFUNCTION()
	void OnInputDeviceChanged(bool IsUsingGamepad);

private:
	bool bClimbing = false;
	FVector ClimbUpVector = FVector::UpVector;

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
	float MaxStamina;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backpack")
	TSubclassOf<ABackPack> BackpackActor;

	bool bInventoryIsOn = false;


protected:
	void ToggleInventory();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSlotAIsInHand = true;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UInventorySlot* GetCurrentlyHeldInventorySlot();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasFreeSpace();

	UFUNCTION(Client, Unreliable)
	void ClearCurrentlyHeldInventorySlot();
	void ClearCurrentlyHeldInventorySlot_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UInventorySlot*> GetAllSlots();

private:
	UInventorySlot* FindFreeSlot();

	UPROPERTY(Replicated)
	AWorldItem* CurrentlyHeldWorldItem;
public:
	AWorldItem* GetCurrentlyHeldWorldItem() const {return this->CurrentlyHeldWorldItem;}
protected:
	UFUNCTION(BlueprintCallable)
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
	void AddMoneyToWallet(int32 Amount);
	void AddMoneyToWallet_Implementation(int32 Amount);

	void DropItem(FSlotData SlotToEmpty, bool bThrow);

public:
	UPROPERTY(BlueprintAssignable)
	FOnItemDrop OnDropItem;


public:
	
	void RemoveItemFromInventorySlot(UInventorySlot* SlotToEmpty);

protected:
	void SwitchHand();

	UFUNCTION()
	void SwitchHandFinished();

	void EquipCurrentInventorySelection(bool BToA);

	void DropItemPressed();

	void ThrowItemPressed();

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


	//hold action
	void TriggerPrimaryHoldItemAction();
	void EndPrimaryHoldItemAction();

	UFUNCTION(Server, Unreliable)
	void Server_TriggerPrimaryItemActionHold();

	UFUNCTION(Server, Unreliable)
	void Server_EndPrimaryItemActionHold();


	//secondary item 
	void TriggerSecondaryItemAction();

	UFUNCTION(Server, Unreliable)
	void Server_TriggerSecondaryItemAction();

private:
	bool bHasBackPack = false;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetHasBackPack() const {return this->bHasBackPack;}

	UFUNCTION(BlueprintCallable)
	void SetHasBackPack(bool bNewHasBackpack);

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
	void SpawnDroppedWorldItem(TSubclassOf<AWorldItem> ItemToSpawn, FTransform SpawnTransform, const FString& SerializedData, bool bThrow, FVector CameraVector);
	UFUNCTION(Server,Reliable)
	void Server_SpawnDroppedWorldItem(TSubclassOf<AWorldItem> ItemToSpawn, FTransform SpawnTransform, const FString& SerializedData, bool bThrow, FVector CameraVector);
	void Server_SpawnDroppedWorldItem_Implementation(TSubclassOf<AWorldItem> ItemToSpawn, FTransform SpawnTransform, const FString& SerializedData, bool bThrow, FVector CameraVector);

	UFUNCTION(Server,Reliable)
	void Server_DropBackpack(const TArray<TSubclassOf<UItemData>>& Items, FTransform SpawnTransform, const  TArray<FString>& SerializedItemDatas);
	void Server_DropBackpack_Implementation(const TArray<TSubclassOf<UItemData>>& Items, FTransform SpawnTransform, const  TArray<FString>& SerializedItemDatas);

public:
	void ReportNoise(float Loudness);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMoveAllowed = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bJumpAllowed=true;
	
	bool bLookAllowed = true;
	bool bSprintAllowed = true;
	bool bPrimaryActionAllowed = true;
	bool bSecondaryActionAllowed = true;

private:
	float OverridenWalkingSpeed;

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

private:
	UFUNCTION(Server, Reliable)
	void Server_EndAttack();
	void Server_EndAttack_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EndAttack();
	void Multicast_EndAttack_Implementation();

private:
	UFUNCTION(Server, Unreliable)
	void Server_DealHits(FWeaponInfo WeaponInfo);
	void Server_DealHits_Implementation(FWeaponInfo WeaponInfo);
public:
	virtual void OnDeath_Implementation() override;

public://cheat Stuff:

	UFUNCTION(BlueprintCallable)
	void Cheat_SpawnItem(TSubclassOf<AWorldItem> ItemToSpawn);
	

public://buyingItems
	void BuyItem(ABuyableItem* ItemToBuy);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Backpack")
	TSubclassOf<UItemData> BackpackClass;



public:
	UFUNCTION(Server, Unreliable)
	void PlaceItemOnSocket(AItemSocket* Socket);
	void PlaceItemOnSocket_Implementation(AItemSocket* Socket);

protected://player hud
	UPlayerHud* MyPlayerHud;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPlayerHud> PlayerHudClass;
public:
	void CreatePlayerHud();

	UFUNCTION(BlueprintPure,BlueprintCallable)
	UPlayerHud* GetMyHud() const {return MyPlayerHud;}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UFootstepSystemComponent* FootstepSystemComponent;

public:
	

	UFUNCTION(BlueprintCallable)
	void dropAllItems();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	USoundBase* JumpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds")
	USoundBase* LandingSound;
	
protected:
	//Basic spawn sound functions (seem to work as long as the reference is in the same object)
	UFUNCTION(Server, Unreliable)
	void Server_SpawnSoundAtLocation(USoundBase* LocalSound, FVector Location);
	void Server_SpawnSoundAtLocation_Implementation(USoundBase* LocalJumpSound, FVector Location);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpawnSoundAtLocation(USoundBase* LocalSound, FVector Location);


public:
	bool bHasNoSprintDebuff=false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsExausted=false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UBuffDebuffBase> NoSprintDebuff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UBuffDebuffBase> ExaustionDebuff;

	void StartExaustionTimer();
	FTimerHandle ExaustionTimer;
	void ApplyExaustion();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Yawn();
	void Yawn_Implementation();

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Sounds")
	USoundBase* InventoryEquipSound;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Sounds")
	USoundBase* DropItemSound;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Sounds")
	USoundBase* FallingToDeathSound;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Sounds")
	USoundBase* ThrowSound;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Sounds")
	USoundBase* YawnSound;

public:
	
	void TakeDamage(float Damage) override;

protected:
	
	UFUNCTION(BlueprintNativeEvent)
	void ShowHudDamageIndicator();

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LeftBehind();
	void LeftBehind_Implementation();

public:
	//exists so the server can drop clients items when they disconnect
	UPROPERTY()
	TArray<FHeldItem> HeldItems;


protected:

	UFUNCTION(BlueprintCallable)
	void UpdateHeldItems();
	
private:
	TArray<FHeldItem> GetHeldItems();
	
	UFUNCTION(Server, Reliable)
	void Server_UpdateHeldItems(const TArray<TSubclassOf<UItemData>>& ItemDataClasses, const TArray<FString>& SerializedItemDatas);
	void Server_UpdateHeldItems_Implementation(const TArray<TSubclassOf<UItemData>>& ItemDataClasses, const TArray<FString>& SerializedItemDatas);
};
