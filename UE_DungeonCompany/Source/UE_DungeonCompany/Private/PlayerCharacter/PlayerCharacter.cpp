// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter/PlayerCharacter.h"
#include "PlayerCharacter/Components/DC_CMC.h"
#include "Components/SceneComponent.h"
#include "DCGame/DC_PC.h"
#include "DC_Statics.h"
#include "UI/PlayerHud/PlayerHud.h"
#include "Items/WorldItem.h"
#include "Items/ItemData.h"
#include "Inventory/Inventory.h"
#include "Inventory/InventorySlot.h"
#include "Items/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Entities/DC_Entity.h"
#include "DCGame/DC_GM.h"
#include "Items/WorldCurrency.h"
#include "WorldActors/SharedStatsManager.h"
#include "Items/BackPack.h"
#include "Items/BuyableItem.h"
#include "Items/Torch_Data.h"
#include "WorldActors/FirstDoorPuzzle/ItemSocket.h"
#include "Engine/World.h"
#include "DCGame/DC_PostMortemPawn.h"
#include "Items/Potion.h"
#include "Items/WorldCurrency_Data.h"
#include "PlayerCharacter/Components/DC_VOIPTalker.h"
#include "EngineUtils.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/VoiceConfig.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "BuffSystem/DebuffExaustion.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Subsystems/VoiceChatSubsystem.h"
#include "DCGame/DC_PostMortemPawn.h"
#include "Items/Potion.h"
#include "Items/SendingStone.h"
#include "Items/WorldCurrency_Data.h"
#include "WorldActors/ResetManager.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDC_CMC>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	FirstPersonMesh->SetupAttachment(RootComponent);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FirstPersonCamera->SetupAttachment(FirstPersonMesh,TEXT("HEAD"));

	FirstPersonCamera->bUsePawnControlRotation = false;

	DropTransform = CreateDefaultSubobject<USceneComponent>(TEXT("DropTransform"));
	DropTransform->SetupAttachment(FirstPersonCamera);
	DropTransform->SetRelativeLocation(FVector(150, 0, -20));


	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;

	static ConstructorHelpers::FObjectFinder<USoundAttenuation> voiceSA(
		TEXT("/Game/_DungeonCompanyContent/Audio/Player/VoiceSA.VoiceSA"));
	VoiceSA = voiceSA.Object;

	VOIPTalker = CreateDefaultSubobject<UDC_VOIPTalker>(TEXT("VOIPTalker"));

	this->Inventory = CreateDefaultSubobject<UInventory>(TEXT("InventoryComponent"));
	this->Backpack = CreateDefaultSubobject<UInventory>(TEXT("BackpackComponent"));

	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
	StimulusSource->RegisterWithPerceptionSystem();

	this->HP = this->MaxHP;
	this->Stamina = this->MaxStamina;

	FootstepSystemComponent = CreateDefaultSubobject<UFootstepSystemComponent>(TEXT("FootstepSystem"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	this->HP = this->MaxHP;
	this->Stamina = this->MaxStamina; //doesnt seem to take the blueprint changes during construction

	GetCharacterMovement()->BrakingDecelerationFlying = 5000.f;
	GetCharacterMovement()->MaxWalkSpeed = this->WalkingSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = this->CrouchedWalkingSpeed;
	GetCharacterMovement<UDC_CMC>()->MaxClimbSpeed = ClimbingSpeed;
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, -1.0f, 0.0f);
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->SetCrouchedHalfHeight(60.f);

	this->HandSlotA = NewObject<UInventorySlot>();
	this->HandSlotB = NewObject<UInventorySlot>();

	VOIPTalker->Settings.AttenuationSettings = VoiceSA;
	VOIPTalker->Settings.ComponentToAttachTo = FirstPersonCamera;

	RestDelegate = FTimerDelegate::CreateLambda([this]()
	{
		bResting = true;
	});

	StartExaustionTimer();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float voiceLevel = VOIPTalker->GetVoiceLevel();

	CheckForFallDamage();

	if (voiceLevel > 0.f)
		ReportNoise(voiceLevel);

	if (IsLocallyControlled())
		LocalTick(DeltaTime);
}

void APlayerCharacter::LocalTick(float DeltaTime)
{
	this->InteractorLineTrace();
	StaminaTick(DeltaTime);
	CheckHoldInteract();
}

void APlayerCharacter::StaminaTick(float DeltaTime)
{
	if (bResting)
		AddStamina(StaminaGainPerSecond * DeltaTime);

	if (!bSprinting)
		return;

	if (GetCharacterMovement()->Velocity.Length() > WalkingSpeed)
		SubstractStamina(SprintStaminaDrainPerSecond * DeltaTime);

	if (Stamina < 0.f)
		ToggleSprint();
}

void APlayerCharacter::Restart()
{
	Super::Restart();

	if (!IsLocallyControlled() || !CharacterInputMapping)
		return;

	ADC_PC* pc = Cast<ADC_PC>(Controller);

	if (!pc)
		return;

	OnInputDeviceChanged(pc->IsUsingGamepad());
	pc->OnInputDeviceChanged.AddDynamic(this, &APlayerCharacter::OnInputDeviceChanged);

	auto inputLocalPlayer = GetInputLocalPlayer();

	if (!inputLocalPlayer)
		return;

	inputLocalPlayer->AddMappingContext(CharacterInputMapping, 0);

	if (!MyPlayerHud)
		CreatePlayerHud();
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, CurrentlyHeldWorldItem);
	DOREPLIFETIME(APlayerCharacter, AttackBlend);
	DOREPLIFETIME(APlayerCharacter, bClimbing);
	DOREPLIFETIME(APlayerCharacter, bIsDrinkingPotion);
}


UEnhancedInputLocalPlayerSubsystem* APlayerCharacter::GetInputLocalPlayer() const
{
	APlayerController* playerController = GetController<APlayerController>();

	if (!playerController)
		return nullptr;

	ULocalPlayer* localPlayer = playerController->GetLocalPlayer();

	if (!localPlayer)
		return nullptr;

	return localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
}

void APlayerCharacter::ActivateCharacterInputMappings()
{
	auto inputLocalPlayer = GetInputLocalPlayer();

	if (!inputLocalPlayer)
		return;

	inputLocalPlayer->AddMappingContext(CharacterInputMapping, 0);

	if (bInventoryIsOn)
		inputLocalPlayer->AddMappingContext(InventoryInputMapping, 1);
}

void APlayerCharacter::DeactivateCharacterInputMappings()
{
	auto inputLocalPlayer = GetInputLocalPlayer();

	if (!inputLocalPlayer)
		return;

	inputLocalPlayer->RemoveMappingContext(CharacterInputMapping);

	if (bInventoryIsOn)
		inputLocalPlayer->RemoveMappingContext(InventoryInputMapping);
}

bool APlayerCharacter::IsCrouchOnHold() const
{
	return bCrouchHold;
}

void APlayerCharacter::SetCrouchHold(bool ShouldHoldCrouch)
{
	bCrouchHold = ShouldHoldCrouch;
}

bool APlayerCharacter::IsSprintOnHold() const
{
	return bSprintHold;
}

void APlayerCharacter::SetSprintHold(bool ShouldHoldSprint)
{
	bSprintHold = ShouldHoldSprint;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!EIC)
		return;

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	EIC->BindAction(MoveAction, ETriggerEvent::None, this, &APlayerCharacter::NoMove);
	EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	EIC->BindAction(LookAction, ETriggerEvent::None, this, &APlayerCharacter::NoLook);
	EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::Jump);
	EIC->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::CrouchActionStarted);
	EIC->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerCharacter::CrouchActionCompleted);
	EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::SprintActionStarted);
	EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::SprintActionCompleted);

	EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::InteractPressed);

	EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::StartHoldInteract);
	EIC->BindAction(InteractAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopHoldInteract);

	EIC->BindAction(InventoryAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleInventory);
	EIC->BindAction(SwitchHandAction, ETriggerEvent::Started, this, &APlayerCharacter::SwitchHand);

	EIC->BindAction(ItemPrimaryAction, ETriggerEvent::Started, this, &APlayerCharacter::TriggerPrimaryItemAction);

	EIC->BindAction(ItemPrimaryAction, ETriggerEvent::Started, this, &APlayerCharacter::TriggerPrimaryHoldItemAction);
	EIC->BindAction(ItemPrimaryAction, ETriggerEvent::Completed, this, &APlayerCharacter::EndPrimaryHoldItemAction);

	EIC->BindAction(ItemSecondaryAction, ETriggerEvent::Started, this, &APlayerCharacter::TriggerSecondaryItemAction);

	EIC->BindAction(DropItemAction, ETriggerEvent::Triggered, this, &APlayerCharacter::DropItemPressed);
	EIC->BindAction(ThrowItemAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ThrowItemPressed);

	EIC->BindAction(ToggleInventoryInvAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleInventory);
	EIC->BindAction(NavigateInventoryAction, ETriggerEvent::Started, this, &APlayerCharacter::NavigateInventory);
	EIC->BindAction(EquipItemInvAction, ETriggerEvent::Started, this, &APlayerCharacter::EquipItem);
	EIC->BindAction(DropItemInvAction, ETriggerEvent::Started, this, &APlayerCharacter::DropItemInvPressed);

	EIC->BindAction(SendingStoneInput, ETriggerEvent::Triggered, this, &APlayerCharacter::SendindStoneInputPressed);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!bMoveAllowed)
		return;

	FVector2D localMoveVector = Value.Get<FVector2D>();

	FVector worldMoveVector = GetActorRightVector() * localMoveVector.X + GetActorForwardVector() * localMoveVector.Y;

	if (bSprinting && (localMoveVector.Y <= 0.f || GetCharacterMovement()->MovementMode == MOVE_Flying))
		StopSprint();

	AddMovementInput(worldMoveVector);
}

void APlayerCharacter::NoMove()
{
	if (bSprinting)
		StopSprint();
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	if (bUsingSelectionWheel)
	{
		MouseValue.X = Value.Get<FVector2d>().X;
		MouseValue.Y = Value.Get<FVector2d>().Y;
		return;
	}
	if (!bLookAllowed)
		return;

	FVector2d lookVector = Value.Get<FVector2d>();
	if (GetCharacterMovement<UDC_CMC>()->IsClimbing())
		lookVector.X = 0.f;

	(*LookFunction)(lookVector, this);

	FRotator newRotation = FRotator(0, 0, 0);
	newRotation.Pitch = GetControlRotation().Euler().Y;

	FirstPersonMesh->SetRelativeRotation(newRotation);
}

void APlayerCharacter::NoLook()
{
	auto pc = Cast<ADC_PC>(Controller);

	if (!pc)
		return;

	pc->SetLastLookVectorLength(0.f);
}

void APlayerCharacter::InteractorLineTrace()
{
	if(!IsValid(MyPlayerHud))
		return;
	
	//raycast to pick up and interact with stuff
	FHitResult Hit;
	FVector Start = this->FirstPersonCamera->GetComponentLocation();
	FVector End = Start + this->FirstPersonCamera->GetForwardVector() * this->InteractionRange;

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility);

	
	if (Hit.bBlockingHit)
	{
		IInteractable* i = Cast<IInteractable>(Hit.GetActor());
		if (i && i->IsInteractable())
		{
			if (CurrentInteractable != i) //if a new intractable is being looked at
			{
				this->CurrentInteractable = i;
				this->CurrentInteractable->OnHovered(this);

				MyPlayerHud->ShowCrosshair(nullptr);
			}
		}
		else
		{
			if (CurrentInteractable != NULL)
			{
				this->MyPlayerHud->HideCrosshair();
				this->CurrentInteractable->OnUnHovered(this);
				this->CurrentInteractable = NULL;
			}
		}
	}
	else
	{
		if (CurrentInteractable != NULL)
		{
			this->MyPlayerHud->HideCrosshair();
			this->CurrentInteractable->OnUnHovered(this);
			this->CurrentInteractable = NULL;
			StopHoldInteract();
		}
	}
}


void APlayerCharacter::DestroyWorldItem(AWorldItem* ItemToDestroy)
{
	if (!HasAuthority())
		Server_DestroyWorldItem(ItemToDestroy);

	Server_DestroyWorldItem_Implementation(ItemToDestroy);
}

void APlayerCharacter::Server_DestroyWorldItem_Implementation(AWorldItem* ItemToDestroy)
{
	ItemToDestroy->OnPickedUp.Broadcast(); //a bit shitty that its here but it should work
	ItemToDestroy->Destroy();
}

void APlayerCharacter::Interact()
{
	if (!CurrentInteractable || !CurrentInteractable->IsInteractable())
		return;

	if (CurrentInteractable->IsInteractionRunningOnServer())
	{
		if (!HasAuthority())
			Server_Interact(Cast<UObject>(CurrentInteractable));
		else
			Server_Interact_Implementation(Cast<UObject>(CurrentInteractable));
	}
	else
	{
		CurrentInteractable->Interact(this);
	}
}

void APlayerCharacter::ResetInteractPrompt()
{
	this->CurrentInteractable = nullptr;
}

void APlayerCharacter::InteractPressed()
{
	if (CurrentInteractable != NULL && !CurrentInteractable->GetNeedsHolding())
		Interact();
}

void APlayerCharacter::StartHoldInteract()
{
	if (CurrentInteractable == NULL || !CurrentInteractable->GetNeedsHolding())
		return;

	this->bIsHoldingInteract = true;
	MyPlayerHud->ShowInteractProgressBar(CurrentInteractable->GetHoldInteractTime());
}

void APlayerCharacter::StopHoldInteract()
{
	if (!this->bIsHoldingInteract)
		return;

	this->bIsHoldingInteract = false;
	InteractHoldingSecondCounter = 0;
	MyPlayerHud->HideInteractProgressBar();
}

void APlayerCharacter::CheckHoldInteract()
{
	if (CurrentInteractable == NULL)
		return;

	if (bIsHoldingInteract)
	{
		InteractHoldingSecondCounter += GetWorld()->GetDeltaSeconds();
		if (CurrentInteractable->GetHoldInteractTime() <= InteractHoldingSecondCounter)
		{
			Interact();
			StopHoldInteract();
		}
	}
}

void APlayerCharacter::Server_Interact_Implementation(UObject* Interactable)
{
	if (!IsValid(Interactable))
		return;

	if (IInteractable* interactableInterface = Cast<IInteractable>(Interactable))
		interactableInterface->AuthorityInteract(this);
}

void APlayerCharacter::PickUpItem(AWorldItem* WorldItem)
{
	if (Cast<AWorldCurrency>(WorldItem))
	{
		DestroyWorldItem(WorldItem);
		Server_SpawnSoundAtLocation(WorldItem->GetPickupSound(), WorldItem->GetRootComponent()->GetComponentLocation());
		this->AddMoneyToWallet(Cast<UWorldCurrency_Data>(Cast<AWorldCurrency>(WorldItem)->GetMyData())->PickUpValue);
		return;
	}

	if (Cast<ABackPack>(WorldItem))
	{
		if (!this->bHasBackPack)
		{
			Server_SpawnSoundAtLocation(WorldItem->GetPickupSound(),
			                            WorldItem->GetRootComponent()->GetComponentLocation());
			PickUpBackpack(Cast<ABackPack>(WorldItem));
		}


		return;
	}

	UInventorySlot* freeSlot = FindFreeSlot();

	if (IsValid(freeSlot))
	{
		Server_SpawnSoundAtLocation(WorldItem->GetPickupSound(), WorldItem->GetRootComponent()->GetComponentLocation());
		//rn only local but maybe that even stays that way
		freeSlot->MyItem = WorldItem->GetMyData();
		DestroyWorldItem(WorldItem);
		UpdateHeldItems();

		if (freeSlot == GetCurrentlyHeldInventorySlot())
			TakeOutItem();

		if (UTorch_Data* torch = Cast<UTorch_Data>(freeSlot->MyItem))
		{
			if (freeSlot != GetCurrentlyHeldInventorySlot())
				torch->bOn = false;
		}
	}
}

void APlayerCharacter::PickUpBackpack(ABackPack* BackpackToPickUp)
{
	SetHasBackPack(true);
	if (bSprinting)
		StopSprint();

	for (int i = 0; i < BackpackToPickUp->Items.Num(); i++)
	{
		if (IsValid(BackpackToPickUp->Items[i]))
		{
			UItemData* TmpItemData = NewObject<UItemData>(GetTransientPackage(), *BackpackToPickUp->Items[i]);
			TmpItemData->DeserializeMyData(BackpackToPickUp->ItemDatas[i]);
			this->Backpack->GetSlots()[i]->MyItem = TmpItemData;
		}
	}

	DestroyWorldItem(BackpackToPickUp);
	if (this->bInventoryIsOn)
		MyPlayerHud->RefreshInventory();
}

void APlayerCharacter::Jump()
{
	if (GetCharacterMovement()->MovementMode != MOVE_Walking)
	{
		Super::Jump();
		this->bJumped = true;
		if (!GetMovementComponent()->IsFalling())
			Server_SpawnSoundAtLocation(this->JumpSound, GetActorLocation());
		return;
	}

	if (Stamina <= 0.f || GetCharacterMovement()->IsCrouching() || !bJumpAllowed)
		return;

	SubstractStamina(JumpStaminaDrain);
	this->bJumped = true;
	Super::Jump();
	if (!GetMovementComponent()->IsFalling())
		Server_SpawnSoundAtLocation(this->JumpSound, GetActorLocation());
}

void APlayerCharacter::CrouchActionStarted()
{
	this->MyPlayerHud->UpdateCrouchIcon();
	if (!bCrouchHold)
	{
		ToggleCrouch();
		return;
	}

	Crouch(true);
}

void APlayerCharacter::CrouchActionCompleted()
{
	this->MyPlayerHud->UpdateCrouchIcon();
	if (!bCrouchHold)
		return;

	UnCrouch(true);
}

void APlayerCharacter::ToggleCrouch()
{
	if (GetCharacterMovement()->IsCrouching())
		UnCrouch(true);
	else
		Crouch(true);
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (IsLocallyControlled())
		this->MyPlayerHud->UpdateCrouchIcon();

	Server_SpawnSoundAtLocation(this->CrouchSound, GetActorLocation());
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (IsLocallyControlled())
		this->MyPlayerHud->UpdateCrouchIcon();

	Server_SpawnSoundAtLocation(this->CrouchSound, GetActorLocation());
}

void APlayerCharacter::SprintActionStarted()
{
	if (!bSprintHold)
	{
		ToggleSprint();
		return;
	}

	StartSprint();
}

void APlayerCharacter::SprintActionCompleted()
{
	if (!bSprintHold)
		return;

	StopSprint();
}

void APlayerCharacter::ToggleSprint()
{
	if (bSprinting)
	{
		StopSprint();
		return;
	}

	StartSprint();
}

void APlayerCharacter::StartSprint()
{
	if (!this->bSprintAllowed || bHasNoSprintDebuff)
		return;

	if (Stamina <= 0.f)
		return;

	GetCharacterMovement<UDC_CMC>()->StartSprint();

	if (!HasAuthority())
		Server_StartSprint();

	Server_StartSprint_Implementation();
}

void APlayerCharacter::Server_StartSprint_Implementation()
{
	//GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed * SprintSpeedMultiplier;
	bSprinting = true;
	//GetCharacterMovement<UDC_CMC>()->StartSprint();
}

void APlayerCharacter::StopSprint()
{
	GetCharacterMovement<UDC_CMC>()->StopSprint();

	if (!HasAuthority())
		Server_StopSprint();

	Server_StopSprint_Implementation();
}

void APlayerCharacter::Server_StopSprint_Implementation()
{
	//GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
	bSprinting = false;
}

void APlayerCharacter::Server_SetActorLocation_Implementation(const FVector& InLocation)
{
	SetActorLocation(InLocation);
}

void APlayerCharacter::Server_LaunchCharacter_Implementation(FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
}

void APlayerCharacter::OnInputDeviceChanged(bool IsUsingGamepad)
{
	LookFunction = IsUsingGamepad ? &UInputFunctionLibrary::LookGamepad : &UInputFunctionLibrary::LookMouse;
}

void APlayerCharacter::AddStamina(float AddingStamina)
{
	if (AddingStamina < 0.f)
	{
		LogWarning("To substract stamina use \"SubstractStamina\" function");
		return;
	}

	if (this->bIsExausted)
	{
		float Factor = Cast<UDebuffExaustion>(ExaustionDebuff.GetDefaultObject())->GetStaminaRecoveryFactor();
		AddingStamina = AddingStamina * Factor;
	}


	Stamina += AddingStamina;

	if (Stamina > MaxStamina)
	{
		Stamina = MaxStamina;
		bResting = false;
	}
}

void APlayerCharacter::SubstractStamina(float SubStamina)
{
	if (SubStamina < 0.f)
	{
		LogWarning("To add stamina use \"AddStamina\" function");
		return;
	}

	bResting = false;
	Stamina -= SubStamina;

	GetWorld()->GetTimerManager().SetTimer(RestDelayTimerHandle, RestDelegate, StaminaGainDelay, false);

	if (Stamina > 0.f)
		return;

	Stamina = 0.f;

	if (bSprinting)
		ToggleSprint();
}

void APlayerCharacter::SetClimbing(bool value)
{
	this->Server_SetClimbing(value);
}

void APlayerCharacter::Server_SetClimbing_Implementation(bool Value)
{
	this->bClimbing = Value;
}

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	return JumpIsAllowedInternal();
}

void APlayerCharacter::SetVoiceEffect(USoundEffectSourcePresetChain* SoundEffect)
{
	VOIPTalker->Settings.SourceEffectChain = SoundEffect;
}

void APlayerCharacter::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);

	if (NewPlayerState)
		VOIPTalker->RegisterWithPlayerState(NewPlayerState);
}


void APlayerCharacter::AddMoneyToWallet_Implementation(int32 Amount)
{
	ASharedStatsManager* wallet = Cast<ASharedStatsManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ASharedStatsManager::StaticClass()));
	wallet->Money += Amount;
	wallet->OnMoneyChanged.Broadcast();
}

void APlayerCharacter::Server_DropBackpack_Implementation(const TArray<TSubclassOf<UItemData>>& Items,
                                                          FTransform SpawnTransform,
                                                          const TArray<FString>& SerializedItemDatas)
{
	ABackPack* a = GetWorld()->SpawnActorDeferred<ABackPack>(BackpackActor, SpawnTransform);
	a->Items = Items;
	a->ItemDatas = SerializedItemDatas;
	a->SetWasDroppedByPlayer(true);
	a->FinishSpawning(SpawnTransform);
}

void APlayerCharacter::ReportNoise(float Loudness)
{
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), Loudness, this);
}

void APlayerCharacter::Cough()
{
	if (HasAuthority())
		ReportNoise(2.f);

	if (CoughSound)
		UGameplayStatics::SpawnSoundAtLocation(this, CoughSound, GetActorLocation());
}

void APlayerCharacter::ToggleInventory()
{
	if (!bSwitchHandAllowed || GetCharacterMovement<UDC_CMC>()->IsClimbing())
		return;

	this->bInventoryIsOn = !bInventoryIsOn;
	this->MyPlayerHud->ToggleInventory(bInventoryIsOn);

	auto inputLocalPlayer = GetInputLocalPlayer();

	if (!inputLocalPlayer)
		return;

	if (bInventoryIsOn)
		inputLocalPlayer->AddMappingContext(InventoryInputMapping, 1);
	else
		inputLocalPlayer->RemoveMappingContext(InventoryInputMapping);
}


UInventorySlot* APlayerCharacter::GetCurrentlyHeldInventorySlot()
{
	if (this->bSlotAIsInHand)
		return HandSlotA;
	else
		return HandSlotB;
}


void APlayerCharacter::ClearCurrentlyHeldInventorySlot_Implementation()
{
	RemoveItemFromInventorySlot(GetCurrentlyHeldInventorySlot());
	//GetCurrentlyHeldInventorySlot()->MyItem = nullptr;
}

TArray<UInventorySlot*> APlayerCharacter::GetAllSlots()
{
	if (!IsLocallyControlled())
	{
		return TArray<UInventorySlot*>();
	}

	TArray<UInventorySlot*> AllSlots;
	AllSlots = this->Inventory->GetSlots();

	if (this->bHasBackPack)
		AllSlots.Append(this->Backpack->GetSlots());

	AllSlots.Add(this->HandSlotA);
	AllSlots.Add(this->HandSlotB);

	return AllSlots;
}

UInventorySlot* APlayerCharacter::FindFreeSlot()
{
	if (bSlotAIsInHand) // a is in hand check a and then b
	{
		if (!IsValid(this->HandSlotA->MyItem))
		{
			return HandSlotA;
		}
		if (!IsValid(this->HandSlotB->MyItem))
		{
			return HandSlotB;
		}
	}
	else //check b and then a
	{
		if (!IsValid(this->HandSlotB->MyItem))
		{
			return HandSlotB;
		}
		if (!IsValid(this->HandSlotA->MyItem))
		{
			return HandSlotA;
		}
	}

	for (UInventorySlot* S : Inventory->GetSlots()) //checking for free inventory slot
	{
		if (!IsValid(S->MyItem))
			return S;
	}

	if (bHasBackPack) //checking for free backpack slots if the player has a backpack
	{
		for (UInventorySlot* S : Backpack->GetSlots())
		{
			if (!IsValid(S->MyItem))
				return S;
		}
	}

	return nullptr;
}

void APlayerCharacter::TakeOutItem()
{
	if (IsValid(CurrentlyHeldWorldItem)) //destroying old item
	{
		DestroyWorldItem(CurrentlyHeldWorldItem);
	}

	UClass* newFirstPersonAnimClass = IsValid(GetCurrentlyHeldInventorySlot()->MyItem)
		                                  ? GetCurrentlyHeldInventorySlot()->MyItem->FirstPersonAnimationBlueprintClass
		                                  : NoItemFirstPersonAnimationBlueprintClass;
	UClass* newThirdPersonAnimClass = IsValid(GetCurrentlyHeldInventorySlot()->MyItem)
		                                  ? GetCurrentlyHeldInventorySlot()->MyItem->ThirdPersonAnimationBlueprintClass
		                                  : NoItemThirdPersonAnimationBlueprintClass;

	if (IsValid(GetCurrentlyHeldInventorySlot()->MyItem)) // if its an item or just a hand
		SpawnItemInHand(GetCurrentlyHeldInventorySlot()->MyItem->MyWorldItemClass,
		                GetCurrentlyHeldInventorySlot()->MyItem->SerializeMyData());

	FirstPersonMesh->AnimClass = NULL; //so the animation blueprint restarts
	FirstPersonMesh->SetAnimClass(newFirstPersonAnimClass);


	if (HasAuthority())
		Multicast_SetTPMeshAnimClass(newThirdPersonAnimClass);
	else
		Server_SetTPMeshAnimClass(newThirdPersonAnimClass);
}

void APlayerCharacter::Server_SetTPMeshAnimClass_Implementation(UClass* NewClass)
{
	Multicast_SetTPMeshAnimClass(NewClass);
}

void APlayerCharacter::Multicast_SetTPMeshAnimClass_Implementation(UClass* NewClass)
{
	if (IsLocallyControlled())
		return;

	GetMesh()->SetAnimClass(NewClass);
}

void APlayerCharacter::SpawnItemInHand(TSubclassOf<AWorldItem> ItemToSpawn, const FString& SerializedData)
{
	if (HasAuthority())
		Server_SpawnItemInHand_Implementation(ItemToSpawn, SerializedData);
	else
		Server_SpawnItemInHand(ItemToSpawn, SerializedData);
}

void APlayerCharacter::Server_SpawnItemInHand_Implementation(TSubclassOf<AWorldItem> ItemToSpawn,
                                                             const FString& SerializedData)
{
	FTransform SpawnTransform;
	CurrentlyHeldWorldItem = GetWorld()->SpawnActorDeferred<AWorldItem>(ItemToSpawn, SpawnTransform);
	CurrentlyHeldWorldItem->MyCharacterToAttachTo = this;
	//this property is replicated and the item will attach on begin play
	CurrentlyHeldWorldItem->SerializedStringData = SerializedData;
	CurrentlyHeldWorldItem->FinishSpawning(SpawnTransform);
	CurrentlyHeldWorldItem->bIsHeldByPlayer = true;
}


void APlayerCharacter::DropItem(FSlotData SlotToEmpty, bool bThrow)
{
	if (SlotToEmpty.bIsBackpackSlot) //
	{
		SetHasBackPack(false);
		if (this->bInventoryIsOn)
			this->MyPlayerHud->RefreshInventory();

		TArray<TSubclassOf<UItemData>> ItemClasses;
		TArray<FString> ItemDatas;

		ItemClasses.SetNum(this->Backpack->NumInventorySlots);
		ItemDatas.SetNum(this->Backpack->NumInventorySlots);

		for (int i = 0; i < this->Backpack->NumInventorySlots; i++)
		{
			if (IsValid(this->Backpack->GetItemAtIndex(i)))
			{
				ItemClasses[i] = this->Backpack->GetItemAtIndex(i)->GetClass();
				ItemDatas[i] = this->Backpack->GetItemAtIndex(i)->SerializeMyData();
				this->Backpack->RemoveItem(i);
			}
		}

		OnDropItem.Broadcast();
		Server_SpawnSoundAtLocation(DropItemSound, this->DropTransform->GetComponentLocation());
		Server_DropBackpack(ItemClasses, GetDropTransform(), ItemDatas);
		return;
	}

	if (IsValid(SlotToEmpty.Slot->MyItem))
	{
		// if (UTorch_Data* torch = Cast<UTorch_Data>(SlotToEmpty.Slot->MyItem))
		// 	if (SlotToEmpty.Slot != GetCurrentlyHeldInventorySlot())
		// 		torch->bOn = false;

		if (bThrow)
			Server_SpawnSoundAtLocation(ThrowSound, this->DropTransform->GetComponentLocation());
		else
			Server_SpawnSoundAtLocation(DropItemSound, this->DropTransform->GetComponentLocation());

		SpawnDroppedWorldItem(SlotToEmpty.Slot->MyItem->MyWorldItemClass, GetDropTransform(),
		                      SlotToEmpty.Slot->MyItem->SerializeMyData(),
		                      bThrow, FirstPersonCamera->GetForwardVector());

		this->RemoveItemFromInventorySlot(SlotToEmpty.Slot);
		OnDropItem.Broadcast();
		UpdateHeldItems();
	}
}

FTransform APlayerCharacter::GetDropTransform()
{
	FVector TraceStart = this->FirstPersonCamera->GetComponentLocation();
	FVector TraceEnd = this->DropTransform->GetComponentLocation();


	FHitResult OutHit;

	// Perform the line trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Visibility);

	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, bHit ? FColor::Green : FColor::Red, false, 3.0f, 0, 1.0f);

	if (bHit)
		return FTransform(DropTransform->GetComponentRotation(), OutHit.Location, FVector(1.0f, 1.0f, 1.0f));
	else
		return FTransform(DropTransform->GetComponentRotation(), TraceEnd, FVector(1.0f, 1.0f, 1.0f));
}

void APlayerCharacter::DropRandomItem()
{
	if (IsLocallyControlled())
	{
		Client_DropRandomItem_Implementation();
		return;
	}

	if (HasAuthority())
		Client_DropRandomItem();
}

void APlayerCharacter::Client_DropRandomItem_Implementation()
{
	auto invSlots = Inventory->GetSlots();

	TArray<UInventorySlot*> filledInventorySlots;

	if (!AttackBlend && GetCurrentlyHeldInventorySlot() != HandSlotA)
		invSlots.Add(HandSlotA);

	if (!AttackBlend && GetCurrentlyHeldInventorySlot() != HandSlotB)
		invSlots.Add(HandSlotB);

	int invSlotsNum = invSlots.Num();

	for (int i = 0; i < invSlotsNum; ++i)
	{
		if (!invSlots[i]->MyItem)
			continue;

		filledInventorySlots.Add(invSlots[i]);
	}

	if (filledInventorySlots.Num() < 1)
		return;

	int droppingSlotIndex = FMath::RandRange(0, filledInventorySlots.Num() - 1);

	if (!filledInventorySlots[droppingSlotIndex])
		return;

	FSlotData dropSlotData;
	dropSlotData.Slot = filledInventorySlots[droppingSlotIndex];
	dropSlotData.bIsBackpackSlot = false;

	DropItem(dropSlotData, false);
}

void APlayerCharacter::RemoveItemFromInventorySlot(UInventorySlot* SlotToEmpty)
{
	if (!IsValid(SlotToEmpty->MyItem))
		return;

	SlotToEmpty->MyItem = nullptr;

	if (GetCurrentlyHeldInventorySlot() == SlotToEmpty)
		TakeOutItem();
}

void APlayerCharacter::SwitchHand()
{
	ADC_PC* pc = GetController<ADC_PC>();

	if (pc && pc->IsUsingGamepad() && bInventoryIsOn)
	{
		DropItem(this->MyPlayerHud->GetHighlightedSlot(), false);
		return;
	}

	if (!bSwitchHandAllowed || bInventoryIsOn || GetCharacterMovement<UDC_CMC>()->IsClimbing())
		return;

	bSwitchHandAllowed = false;
	this->bPrimaryActionAllowed = false;
	this->bSecondaryActionAllowed = false;

	this->bSlotAIsInHand = !bSlotAIsInHand;

	//when switching to torch, it should always be turned off
	if (UTorch_Data* torch = Cast<UTorch_Data>(GetCurrentlyHeldInventorySlot()->MyItem))
		torch->bOn = false;

	//refreshing the prompt message
	this->ResetInteractPrompt();
	TakeOutItem();

	this->MyPlayerHud->OnSwichingDone.AddDynamic(this, &APlayerCharacter::SwitchHandFinished);
	this->MyPlayerHud->SwichHandDisplays(bSlotAIsInHand);
}

void APlayerCharacter::SwitchHandFinished()
{
	bSwitchHandAllowed = true;
	this->bPrimaryActionAllowed = true;
	this->bSecondaryActionAllowed = true;
	MyPlayerHud->OnSwichingDone.RemoveAll(this);
}

void APlayerCharacter::EquipCurrentInventorySelection(bool BToA)
{
	if (MyPlayerHud->GetHighlightedSlot().bIsBackpackSlot)
		return;

	if (UTorch_Data* torch = Cast<UTorch_Data>(MyPlayerHud->GetHighlightedSlot().Slot->MyItem))
		//taken out torch is turned off
		torch->bOn = false;

	UInventorySlot* slot;

	if (BToA)
		slot = HandSlotA;
	else
		slot = HandSlotB;

	if (UTorch_Data* torch = Cast<UTorch_Data>(slot->MyItem)) //storedTorch is turned off
		torch->bOn = false;

	if (UTorch_Data* torch = Cast<UTorch_Data>(MyPlayerHud->GetHighlightedSlot().Slot->MyItem))
		torch->bOn = false;

	//switch
	UItemData* tmp = MyPlayerHud->GetHighlightedSlot().Slot->MyItem;
	MyPlayerHud->GetHighlightedSlot().Slot->MyItem = slot->MyItem;
	slot->MyItem = tmp;

	UGameplayStatics::PlaySound2D(GetWorld(), InventoryEquipSound);


	if (GetCurrentlyHeldInventorySlot() == slot) //if equipping to slot in hand
	{
		TakeOutItem();
	}
}

void APlayerCharacter::DropItemPressed()
{
	if (AttackBlend || bIsUsingSendingStone)
		return;

	FSlotData SD;
	SD.Slot = GetCurrentlyHeldInventorySlot();
	SD.bIsBackpackSlot = false;
	DropItem(SD, false);
}

void APlayerCharacter::ThrowItemPressed()
{
	if (AttackBlend || bIsUsingSendingStone)
		return;

	FSlotData SD;
	SD.Slot = GetCurrentlyHeldInventorySlot();
	SD.bIsBackpackSlot = false;
	DropItem(SD, true);
}

void APlayerCharacter::TriggerPrimaryItemAction()
{
	if (bInventoryIsOn || !bPrimaryActionAllowed || !IsValid(CurrentlyHeldWorldItem))
		return;

	CurrentlyHeldWorldItem->TriggerLocalPrimaryAction(this);

	if (HasAuthority())
	{
		Server_TriggerPrimaryItemAction_Implementation();
		return;
	}

	Server_TriggerPrimaryItemAction();
}


void APlayerCharacter::TriggerPrimaryHoldItemAction()
{
	if (bInventoryIsOn || !bPrimaryActionAllowed || !IsValid(CurrentlyHeldWorldItem))
		return;

	CurrentlyHeldWorldItem->TriggerLocalPrimaryActionHold(this);

	if (HasAuthority())
	{
		Server_TriggerPrimaryItemActionHold_Implementation();
		return;
	}

	Server_TriggerPrimaryItemActionHold();
}

void APlayerCharacter::EndPrimaryHoldItemAction()
{
	if (bInventoryIsOn || !bPrimaryActionAllowed || !IsValid(CurrentlyHeldWorldItem))
		return;

	CurrentlyHeldWorldItem->EndLocalPrimaryActionHold(this);

	if (HasAuthority())
	{
		Server_EndPrimaryItemActionHold_Implementation();
		return;
	}

	Server_EndPrimaryItemActionHold();
}


bool APlayerCharacter::HasFreeSpace()
{
	return IsValid(this->FindFreeSlot());
}

void APlayerCharacter::Server_EndPrimaryItemActionHold_Implementation()
{
	if (!bPrimaryActionAllowed || !IsValid(CurrentlyHeldWorldItem))
		return;

	CurrentlyHeldWorldItem->EndPrimaryActionHold(this);
}

void APlayerCharacter::TriggerSecondaryItemAction()
{
	if (bInventoryIsOn || !bSecondaryActionAllowed || !IsValid(CurrentlyHeldWorldItem))
		return;

	CurrentlyHeldWorldItem->TriggerLocalSecondaryAction(this);

	if (HasAuthority())
	{
		Server_TriggerSecondaryItemAction_Implementation();
		return;
	}

	Server_TriggerSecondaryItemAction();
}

void APlayerCharacter::SetHasBackPack(bool bNewHasBackpack)
{
	this->bHasBackPack = bNewHasBackpack;

	if(!this->IsA(Cast<ADC_GM>(GetWorld()->GetAuthGameMode())->PlayerclassFarmer))
	{
		if (bNewHasBackpack)
			this->AddBuffOrDebuff(NoSprintDebuff);
		else
			this->RemoveBuffOrDebuff(NoSprintDebuff);	
	}
	
}

void APlayerCharacter::SpawnDroppedWorldItem(TSubclassOf<AWorldItem> ItemToSpawn, FTransform SpawnTransform,
                                             const FString& SerializedData, bool bThrow, FVector CameraVector)
{
	if (!HasAuthority())
		Server_SpawnDroppedWorldItem(ItemToSpawn, SpawnTransform, SerializedData, bThrow, CameraVector);
	else
		Server_SpawnDroppedWorldItem_Implementation(ItemToSpawn, SpawnTransform, SerializedData, bThrow, CameraVector);
}

void APlayerCharacter::Server_SpawnDroppedWorldItem_Implementation(TSubclassOf<AWorldItem> ItemToSpawn,
                                                                   FTransform SpawnTransform,
                                                                   const FString& SerializedData, bool bThrow,
                                                                   FVector CameraVector)
{
	AWorldItem* i = GetWorld()->SpawnActorDeferred<AWorldItem>(ItemToSpawn, SpawnTransform);
	i->SerializedStringData = SerializedData;
	i->SetWasDroppedByPlayer(true);
	i->FinishSpawning(SpawnTransform);

	if (bThrow)
	{
		if (UMeshComponent* u = Cast<UMeshComponent>(i->GetRootComponent()))
		{
			u->AddImpulse(CameraVector * this->ThrowStrengh * u->GetMass());
		}
	}
}

void APlayerCharacter::CheckForFallDamage()
{
	// i am using Velocity.z instead of movementComponent::IsFalling() because it already counts as falling when the player is in the air while jumping. 
	// that results in the jump height not being included in the fall height calculation

	if (GetMovementComponent()->Velocity.Z == 0 && BWasFallingInLastFrame) //frame of impact
	{
		this->bJumped = false; //ignore me i am only for the animation blueprint
		float deltaZ = LastStandingHeight - this->RootComponent->GetComponentLocation().Z + 20;
		//+20 artificially because the capsule curvature lets the player stand lower

		Server_SpawnSoundAtLocation(LandingSound, this->GetActorLocation());
		float damage = this->FallDamageCalculation(deltaZ);
		if (damage > 0)
		{
			if (damage >= this->HP)
				Server_SpawnSoundAtLocation(FallingToDeathSound, this->GetActorLocation());

			TakeDamage(damage);
		}

		//FString message = 
		//	"\n\nStart height:\t"+FString::SanitizeFloat(LastStandingHeight)+
		//	"\nEnd height:\t"+FString::SanitizeFloat(RootComponent->GetComponentLocation().Z)
		//	+ "\nFall height:\t " + FString::SanitizeFloat(deltaZ);
		//LogWarning(*message);
	}
	if (GetMovementComponent()->Velocity.Z >= 0 || !GetCharacterMovement()->IsFalling())
		LastStandingHeight = this->RootComponent->GetComponentLocation().Z;

	this->BWasFallingInLastFrame = (GetMovementComponent()->Velocity.Z < 0 && GetCharacterMovement()->IsFalling());
}


float APlayerCharacter::FallDamageCalculation(float deltaHeight)
{
	const float free = 300;
	const float max = 1000;

	if (deltaHeight <= free)
		return 0;


	float factor = 100 / (max - free);

	return (deltaHeight - free) * factor;
}

void APlayerCharacter::NavigateInventory(const FInputActionValue& Value)
{
	FVector2D input = Value.Get<FVector2D>();


	if (!MyPlayerHud || bIsUsingSendingStone)
		return;


	if (input.Y != 0)
	{
		MyPlayerHud->MoveHighlight(input.Y > 0 ? EDirections::Up : EDirections::Down);
	}

	if (input.X != 0)
	{
		MyPlayerHud->MoveHighlight(input.X > 0 ? EDirections::Right : EDirections::Left);
	}
}

void APlayerCharacter::EquipItem(const FInputActionValue& Value)
{
	bool bToA = Value.Get<float>() < 0.f;

	EquipCurrentInventorySelection(bToA);
}

void APlayerCharacter::DropItemInvPressed()
{
	ADC_PC* pc = GetController<ADC_PC>();

	if (!pc)
		return;

	DropItem(this->MyPlayerHud->GetHighlightedSlot(), false);
}


void APlayerCharacter::OnDeath_Implementation()
{
	Super::OnDeath_Implementation();

	VOIPTalker->DestroyComponent();

	GetCapsuleComponent()->SetCollisionProfileName("DeadPawn", true);
	StimulusSource->DestroyComponent(false);

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	GetMesh()->SetAnimation(nullptr);

	// GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	// GetMesh()->SetAllBodiesSimulatePhysics(true);
	// GetMesh()->SetSimulatePhysics(true);
	// GetMesh()->WakeAllRigidBodies();

	GetMesh()->SetVisibility(false);

	if (HasAuthority())
		SpawnCorpse();

	if (IsLocallyControlled())
	{
		this->MyPlayerHud->RemoveFromParent();
		DeactivateCharacterInputMappings();
		DropAllItems();
		if (IsValid(CurrentlyHeldWorldItem))
			DestroyWorldItem(CurrentlyHeldWorldItem);

		GetGameInstance()->GetSubsystem<UVoiceChatSubsystem>()->UnmuteAllPlayers();
	}


	if (!HasAuthority())
		return;


	//check if all players are dead
	//assumes authority
	bool bAllDead = true;
	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		if (!It->IsDead())
		{
			bAllDead = false;
			break;
		}
	}

	if (bAllDead)
	{
		if (AResetManager* ResetManager = Cast<AResetManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), AResetManager::StaticClass())))
			ResetManager->Server_ResetDungeon();

		return;
	}


	GetWorld()->GetAuthGameMode<ADC_GM>()->StartSpectating(GetController());
}


void APlayerCharacter::Server_TriggerPrimaryItemAction_Implementation()
{
	if (!bPrimaryActionAllowed || !IsValid(CurrentlyHeldWorldItem))
		return;

	CurrentlyHeldWorldItem->TriggerPrimaryAction(this);
}

void APlayerCharacter::Server_TriggerPrimaryItemActionHold_Implementation()
{
	if (!bPrimaryActionAllowed || !IsValid(CurrentlyHeldWorldItem))
		return;

	CurrentlyHeldWorldItem->TriggerPrimaryActionHold(this);
}

void APlayerCharacter::Server_TriggerSecondaryItemAction_Implementation()
{
	if (!bSecondaryActionAllowed || !IsValid(CurrentlyHeldWorldItem))
		return;

	CurrentlyHeldWorldItem->TriggerSecondaryAction(this);
}

void APlayerCharacter::StartAttacking()
{
	if (!HasAuthority())
		Server_AttackStart();
	else
		Multicast_AttackStart();
}

void APlayerCharacter::AttackStart()
{
	if (GetCharacterMovement<UDC_CMC>()->IsClimbing())
		return;

	if (AttackBlend != 0 || this->Stamina <= 0) //so a new attack only stars when the old one is already over
		return;
	//different attack when sprinting?
	//attack needs to cost stamina
	LogWarning(TEXT("Attack Start"));
	this->bPrimaryActionAllowed = false;
	this->AttackBlend = 1;
	this->bSwitchHandAllowed = false;

	this->bSprintAllowed = false;

	OverridenWalkingSpeed = WalkingSpeed;
	GetCharacterMovement()->MaxWalkSpeed = SlowedWalkingSpeed;

	if (IsLocallyControlled())
	{
		StopSprint();
		SubstractStamina(Cast<AWeapon>(CurrentlyHeldWorldItem)->GetStaminaCost());
	}
}


void APlayerCharacter::Server_AttackStart_Implementation()
{
	Multicast_AttackStart();
}

void APlayerCharacter::Multicast_AttackStart_Implementation()
{
	if (IsLocallyControlled())
		return;

	AttackStart();
}

void APlayerCharacter::Cheat_SpawnItem(TSubclassOf<AWorldItem> ItemToSpawn)
{
	Server_SpawnDroppedWorldItem(ItemToSpawn, GetDropTransform(), FString(), false,
	                             FVector(0, 0, 0));
}

void APlayerCharacter::AttackLanded()
{
	AWeapon* weapon = Cast<AWeapon>(CurrentlyHeldWorldItem);
	FWeaponInfo info = weapon->GetWeaponInfo();
	Server_DealHits(info);
}


void APlayerCharacter::Server_DealHits_Implementation(FWeaponInfo WeaponInfo)
{
	AWeapon* weapon = Cast<AWeapon>(CurrentlyHeldWorldItem);
	weapon->DealHits(WeaponInfo);
}

void APlayerCharacter::OnAttackOver()
{
	if (HasAuthority())
		Multicast_EndAttack();
	else if (IsLocallyControlled())
		Server_EndAttack();
}

void APlayerCharacter::Server_EndAttack_Implementation()
{
	Multicast_EndAttack();
}

void APlayerCharacter::Multicast_EndAttack_Implementation()
{
	this->AttackBlend = 0;
	this->bSwitchHandAllowed = true;
	//this->bMoveAllowed = true;
	//this->bLookAllowed = true;
	this->bPrimaryActionAllowed = true;
	LogWarning(TEXT("Attack END"));
	//allow sprint
	this->bSprintAllowed = true;
	WalkingSpeed = OverridenWalkingSpeed;
	GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
}

void APlayerCharacter::BuyItem(ABuyableItem* ItemToBuy)
{
	Server_SpawnSoundAtLocation(BuySpound, ItemToBuy->GetActorLocation());

	if (ItemToBuy->MyItemDataClass == BackpackClass)
	{
		if (!this->bHasBackPack)
		{
			AddMoneyToWallet(-ItemToBuy->GetPrice());
			SetHasBackPack(true);
		}
		return;
	}

	AddMoneyToWallet(-ItemToBuy->GetPrice());
	UInventorySlot* freeSlot = FindFreeSlot();

	if (IsValid(freeSlot))
	{
		freeSlot->MyItem = NewObject<UItemData>(GetTransientPackage(), ItemToBuy->MyItemDataClass);

		if (freeSlot == GetCurrentlyHeldInventorySlot())
			TakeOutItem();
	}
}


void APlayerCharacter::PlaceItemOnSocket_Implementation(AItemSocket* Socket)
{
	Socket->OnItemPlaced();
}

void APlayerCharacter::CreatePlayerHud()
{
	if (!IsLocallyControlled())
		return;

	this->MyPlayerHud = CreateWidget<UPlayerHud>(GetWorld(), PlayerHudClass);
	this->MyPlayerHud->AddToViewport();
}

void APlayerCharacter::DropAllItems()
{
	if (IsLocallyControlled())
	{
		TArray<UInventorySlot*> AllSlots = GetAllSlots();

		for (UInventorySlot* IS : AllSlots)
		{
			if (IsValid(IS->MyItem))
			{
				UItemData* data = IS->MyItem;
				SpawnDroppedWorldItem(data->MyWorldItemClass, GetDropTransform(),
				                      data->SerializeMyData(), false, FVector::Zero());
			}
		}
		if (bHasBackPack)
		{
			//backpack is spawning without items in it. Its items drop like the others
			Server_DropBackpack(TArray<TSubclassOf<UItemData>>(), GetDropTransform(),
			                    TArray<FString>());
		}
	}
	else
	{
		for (FHeldItem HeldItem : this->HeldItems)
		{
			LogWarning("TryingToSpawnDroppedItem...");
			SpawnDroppedWorldItem(HeldItem.ItemDataClass.GetDefaultObject()->MyWorldItemClass,
			                      GetDropTransform(), HeldItem.ItemData, false, FVector::Zero());
		}
	}
}


void APlayerCharacter::Server_SpawnSoundAtLocation_Implementation(USoundBase* LocalSound, FVector Location)
{
	Multicast_SpawnSoundAtLocation(LocalSound, Location);
}

void APlayerCharacter::StartExaustionTimer()
{
	float Time = Cast<UDebuffExaustion>(ExaustionDebuff.GetDefaultObject())->GetTimeUntulExaustion();
	GetWorld()->GetTimerManager().SetTimer(ExaustionTimer, this, &APlayerCharacter::ApplyExaustion, Time, false);
}

void APlayerCharacter::ApplyExaustion()
{
	this->AddBuffOrDebuff(ExaustionDebuff);
	Yawn();
}

void APlayerCharacter::Yawn_Implementation()
{
}

void APlayerCharacter::TakeDamage(float Damage)
{
	Super::TakeDamage(Damage);
	ShowHudDamageIndicator();
}


void APlayerCharacter::LeftBehind_Implementation()
{
}

TArray<FHeldItem> APlayerCharacter::GetHeldItems()
{
	TArray<FHeldItem> TemporaryArray;
	for (UInventorySlot* s : GetAllSlots())
	{
		if (!IsValid(s->MyItem))
			continue;

		FHeldItem TemporaryHeldItem;
		TemporaryHeldItem.ItemDataClass = s->MyItem->GetClass();
		TemporaryHeldItem.ItemData = s->MyItem->SerializeMyData();
		TemporaryArray.Add(TemporaryHeldItem);
	}
	if (bHasBackPack)
	{
		FHeldItem backpack;
		backpack.ItemDataClass = BackpackClass;
		backpack.ItemData = " ";
		TemporaryArray.Add(backpack);
	}
	return TemporaryArray;
}

void APlayerCharacter::UpdateHeldItems()
{
	TArray<FHeldItem> heldItems = GetHeldItems();

	TArray<TSubclassOf<UItemData>> ItemDataClasses;
	TArray<FString> SerializedItemDatas;

	for (FHeldItem tmp : heldItems)
	{
		ItemDataClasses.Add(tmp.ItemDataClass);
		SerializedItemDatas.Add(tmp.ItemData);
	}

	if (HasAuthority())
		Server_UpdateHeldItems_Implementation(ItemDataClasses, SerializedItemDatas);
	else
		Server_UpdateHeldItems(ItemDataClasses, SerializedItemDatas);
}

void APlayerCharacter::Server_UpdateHeldItems_Implementation(const TArray<TSubclassOf<UItemData>>& ItemDataClasses,
                                                             const TArray<FString>& SerializedItemDatas)
{
	TArray<FHeldItem> newHeldItems = TArray<FHeldItem>();

	for (int i = 0; i < ItemDataClasses.Num(); i++)
	{
		FHeldItem tmpHeldItem;
		tmpHeldItem.ItemDataClass = ItemDataClasses[i];
		tmpHeldItem.ItemData = SerializedItemDatas[i];
		newHeldItems.Add(tmpHeldItem);
	}

	this->HeldItems = newHeldItems;
}

UUserWidget* APlayerCharacter::StartSelectionWheel(TArray<FString> Options)
{
	bMoveAllowed = false;
	bLookAllowed = false;
	bJumpAllowed = false;
	bSwitchHandAllowed = false;

	bUsingSelectionWheel = true;
	return GetMyHud()->ShowSelectionWheel(Options);
}

int APlayerCharacter::EndSelectionWheel()
{
	bMoveAllowed = true;
	bLookAllowed = true;
	bJumpAllowed = true;
	bSwitchHandAllowed = true;

	bUsingSelectionWheel = false;
	return GetMyHud()->DestroySelectionWheel();
}

void APlayerCharacter::OnPotionDrunk()
{
	Server_OnPotionDrunk();
	
	if (APotion* Potion = Cast<APotion>(GetCurrentlyHeldWorldItem()))
	{
		Potion->Local_ApplyEffect(this);
	}
}

void APlayerCharacter::StartDrinkingPotion()
{
	this->bIsDrinkingPotion = true;
	this->bSwitchHandAllowed = false;
	this->bPrimaryActionAllowed = false;
}

void APlayerCharacter::StopDrinkingPotion()
{
	this->bIsDrinkingPotion = false;
	this->bSwitchHandAllowed = true;
	this->bPrimaryActionAllowed = true;
	RemoveItemFromInventorySlot(GetCurrentlyHeldInventorySlot());
}

void APlayerCharacter::Server_OnPotionDrunk_Implementation()
{
	if (APotion* Potion = Cast<APotion>(GetCurrentlyHeldWorldItem()))
	{
		Potion->Authority_ApplyEffect(this);
	}
}

void APlayerCharacter::SpawnCorpse()
{
	if (HasAuthority())
		Server_SpawnCorpse_Implementation();
	else
		Server_SpawnCorpse();
}

void APlayerCharacter::Server_SpawnCorpse_Implementation()
{
	if (IsValid(CorpseClass) && HasAuthority())
	{
		GetWorld()->SpawnActor<AActor>(CorpseClass, this->GetActorTransform());
	}
}

void APlayerCharacter::SendindStoneInputPressed(const FInputActionValue& Value)
{
	if (!bIsUsingSendingStone || SendingStoneAnimatoinState != ESendingStoneAnimatoinState::Neutral)
		return;

	FVector2d InputValue = Value.Get<FVector2d>();
	if (InputValue.X == 1)
	{
		this->SendingStoneAnimatoinState = ESendingStoneAnimatoinState::Up;
		return;
	}
	if (InputValue.X == -1)
	{
		this->SendingStoneAnimatoinState = ESendingStoneAnimatoinState::Down;
		return;
	}

	if (InputValue.Y == 1)
	{
		this->SendingStoneAnimatoinState = ESendingStoneAnimatoinState::Right;
		return;
	}

	if (InputValue.Y == -1)
	{
		this->SendingStoneAnimatoinState = ESendingStoneAnimatoinState::Left;
		return;
	}
}

void APlayerCharacter::SendSendingStoneSignal(ESendingStoneAnimatoinState Signal)
{
	if (HasAuthority())
		Server_SendSendingStoneSignal_Implementation(Signal);
	else
		Server_SendSendingStoneSignal(Signal);
}

void APlayerCharacter::Server_SendSendingStoneSignal_Implementation(ESendingStoneAnimatoinState Signal)
{
	if (ASendingStone* SendingStone = Cast<ASendingStone>(GetCurrentlyHeldWorldItem()))
	{
		SendingStone->Server_SendSignal(Signal);
	}
}

bool APlayerCharacter::HasItemOfClass(TSubclassOf<UItemData> Item)
{
	for (FHeldItem f : this->HeldItems)
	{
		if (f.ItemDataClass == Item)
			return true;
	}
	return false;
}

void APlayerCharacter::Destroyed()
{
	if(IsValid(MyPlayerHud))
		MyPlayerHud->RemoveFromParent();
	
	Super::Destroyed();
}

void APlayerCharacter::SetAttackBlend(float NewBlend)
{
	if(HasAuthority())
		Server_SetAttackBlend_Implementation(NewBlend);
	else
		Server_SetAttackBlend(NewBlend);
}

void APlayerCharacter::Server_SetAttackBlend_Implementation(float NewBlend)
{
	this->AttackBlend=NewBlend;
}

void APlayerCharacter::TransferInventory_Implementation(APlayerCharacter* OldCharacter)
{
	if(!IsValid(OldCharacter))
		return;
	
	this->SetHasBackPack(OldCharacter->bHasBackPack); 

	for (int i = 0; i < OldCharacter->Inventory->GetSlots().Num(); i++)
	{
		if (!IsValid(OldCharacter->Inventory->GetSlots()[i]->MyItem))
			continue;

		this->Inventory->GetSlots()[i]->MyItem = OldCharacter->GetInventory()->GetSlots()[i]->MyItem;
	}

	if(this->bHasBackPack)
	{
		for (int i = 0; i < OldCharacter->Backpack->GetSlots().Num(); i++)
		{
			if (!IsValid(OldCharacter->Backpack->GetSlots()[i]->MyItem))
				continue;

			this->GetBackpack()->GetSlots()[i]->MyItem = OldCharacter->Backpack->GetSlots()[i]->MyItem;
		}	
	}

	if(IsValid(OldCharacter->HandSlotA->MyItem))
		this->HandSlotA->MyItem=OldCharacter->HandSlotA->MyItem;

	if(IsValid(OldCharacter->HandSlotB->MyItem))
	this->HandSlotB->MyItem=OldCharacter->HandSlotB->MyItem;
	
	TakeOutItem();

	OldCharacter->SelfDestruct();
}

void APlayerCharacter::SelfDestruct_Implementation()
{
	this->Destroy(true, true);
}

void APlayerCharacter::ShowHudDamageIndicator_Implementation()
{
}

void APlayerCharacter::Multicast_SpawnSoundAtLocation_Implementation(USoundBase* LocalSound, FVector Location)
{
	if (IsValid(LocalSound))
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), LocalSound, Location);


	FString text = "playing at: " + this->GetRootComponent()->GetComponentLocation().ToString();

	if (HasAuthority())
		text = "server: " + text;
	else
		text = "client: " + text;

	LogWarning(*text);
}
