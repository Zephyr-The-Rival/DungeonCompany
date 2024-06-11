// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter/PlayerCharacter.h"
#include "PlayerCharacter/Components/DC_CMC.h"
#include "DCGame/DC_PC.h"
#include "DC_Statics.h"
#include "UI/PlayerHud/PlayerHud.h"
#include "Items/WorldItem.h"
#include "Items/ItemData.h"
#include "Inventory/Inventory.h"
#include "Inventory/InventorySlot.h"
#include "Items/Weapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Entities/DC_Entity.h"
#include "DCGame/DC_GM.h"
#include "Items/WorldCurrency.h"
#include "WorldActors/SharedStatsManager.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/VoiceConfig.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Interfaces/VoiceInterface.h"
#include "Kismet/GameplayStatics.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDC_CMC>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	FirstPersonMesh->SetupAttachment(RootComponent);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FirstPersonCamera->SetupAttachment(FirstPersonMesh,TEXT("HEAD"));
	//FirstPersonCamera->bUsePawnControlRotation = true;

	GetCharacterMovement()->BrakingDecelerationFlying = 5000.f;
	GetCharacterMovement()->MaxWalkSpeed = this->WalkingSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = this->CrouchedWalkingSpeed;
	GetCharacterMovement()->MaxFlySpeed = ClimbingSpeed;
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, -1.0f, 0.0f);
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(60.f);

	static ConstructorHelpers::FObjectFinder<USoundAttenuation> voiceSA(TEXT("/Game/_DungeonCompanyContent/Audio/Player/VoiceSA.VoiceSA"));
	VoiceSA = voiceSA.Object;

	VOIPTalker = CreateDefaultSubobject<UVOIPTalker>(TEXT("VOIPTalker"));

	this->Inventory = CreateDefaultSubobject<UInventory>(TEXT("InventoryComponent"));
	this->Backpack = CreateDefaultSubobject<UInventory>(TEXT("BackpackComponent"));

	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
	StimulusSource->RegisterWithPerceptionSystem();

	this->HP = this->MaxHP;
	this->Stamina = this->MaxStamina;


}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//this did not work on begin play
	this->HandSlotA = NewObject<UInventorySlot>();
	this->HandSlotB = NewObject<UInventorySlot>();

	VOIPTalker->Settings.AttenuationSettings = VoiceSA;
	VOIPTalker->Settings.ComponentToAttachTo = FirstPersonCamera;

	RestDelegate = FTimerDelegate::CreateLambda([this](){
	bResting = true; 
	});

	if(!IsLocallyControlled() || !CharacterInputMapping)
		return;

	auto inputLocalPlayer = GetInputLocalPlayer();

	if(!inputLocalPlayer)
		return;

	inputLocalPlayer->AddMappingContext(CharacterInputMapping, 0);
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float voiceLevel = VOIPTalker->GetVoiceLevel();

	CheckForFallDamage();

	if(voiceLevel > 0.f)
		ReportTalking(voiceLevel);
	
	if(IsLocallyControlled())
		LocalTick(DeltaTime);

}

void APlayerCharacter::LocalTick(float DeltaTime)
{
	this->InteractorLineTrace();
	StaminaTick(DeltaTime);
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

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, CurrentlyHeldWorldItem);
	DOREPLIFETIME(APlayerCharacter, AttackBlend);
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

	if(!inputLocalPlayer)
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

	if(!EIC)
		return;

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	EIC->BindAction(MoveAction, ETriggerEvent::None, this, &APlayerCharacter::NoMove);
	EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::Jump);
	EIC->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::CrouchActionStarted);
	EIC->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerCharacter::CrouchActionCompleted);
	EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::SprintActionStarted);
	EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::SprintActionCompleted);

	EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::Interact);

	EIC->BindAction(InventoryAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleInventory);
	EIC->BindAction(SwitchHandAction, ETriggerEvent::Started, this, &APlayerCharacter::SwitchHand);	

	EIC->BindAction(ItemPrimaryAction, ETriggerEvent::Started, this, &APlayerCharacter::TriggerPrimaryItemAction);
	EIC->BindAction(ItemSecondaryAction, ETriggerEvent::Started, this, &APlayerCharacter::TriggerSecondaryItemAction);
	
	EIC->BindAction(DropItemAction,	ETriggerEvent::Triggered, this, &APlayerCharacter::DropItemPressed);

	EIC->BindAction(NavigateInventoryAction, ETriggerEvent::Started, this, &APlayerCharacter::NavigateInventory);
	EIC->BindAction(EquipItemInvAction, ETriggerEvent::Started, this, &APlayerCharacter::EquipItem);
	EIC->BindAction(DropItemInvAction, ETriggerEvent::Started, this, &APlayerCharacter::DropItemInvPressed);

		
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!bMoveAllowed)
		return;
	
	FVector2D localMoveVector = Value.Get<FVector2D>();

	FVector worldMoveVector;

	if (GetCharacterMovement()->MovementMode != MOVE_Flying)
		worldMoveVector = GetActorRightVector() * localMoveVector.X + GetActorForwardVector() * localMoveVector.Y;
	else
		worldMoveVector = ClimbUpVector * localMoveVector.Y;

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
	if (bLookAllowed)
	{
		FVector2D lookVector = Value.Get<FVector2D>();

		AddControllerYawInput(lookVector.X);

		AddControllerPitchInput(lookVector.Y);
		FRotator newRotation = FRotator(0, 0, 0);
		newRotation.Pitch = GetControlRotation().Euler().Y;

		FirstPersonMesh->SetRelativeRotation(newRotation);
	}
}

void APlayerCharacter::InteractorLineTrace()
{
	//raycast to pick up and interact with stuff
	FHitResult Hit;
	float distance = 150;
	FVector Start = this->FirstPersonCamera->GetComponentLocation();
	FVector End = Start + this->FirstPersonCamera->GetForwardVector() * this->InteractionRange;

	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility);

	if (Hit.bBlockingHit)
	{
		IInteractable* i = Cast<IInteractable>(Hit.GetActor());
		if (i && i->IsInteractable())
		{
			if (CurrentInteractable != i)//if a new intractable is being looked at
			{
			this->CurrentInteractable = i;
				
				ADC_PC* c = Cast<ADC_PC>(GetController());
				c->GetMyPlayerHud()->ShowCrosshair(TEXT("to Interact"));
			}
		}
		else
		{
			if (CurrentInteractable != NULL)
			{
				Cast<ADC_PC>(GetController())->GetMyPlayerHud()->HideCrosshair();
			}
			this->CurrentInteractable = NULL;
		}
		

	}
	else
	{
		if (CurrentInteractable != NULL)
		{
			Cast<ADC_PC>(GetController())->GetMyPlayerHud()->HideCrosshair();
		}
		this->CurrentInteractable = NULL;
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
	ItemToDestroy->Destroy();
}

void APlayerCharacter::Interact()
{
	if(!CurrentInteractable || !CurrentInteractable->IsInteractable())
		return;

	CurrentInteractable->Interact(this);

	if (!CurrentInteractable->IsInteractionRunningOnServer())
		return;
	

	if(!HasAuthority())
		Server_Interact(Cast<UObject>(CurrentInteractable));
	else
		Server_Interact_Implementation(Cast<UObject>(CurrentInteractable));

}

void APlayerCharacter::Server_Interact_Implementation(UObject* Interactable)
{
	if(!IsValid(Interactable))
		return;

	if (IInteractable* interactableInterface = Cast<IInteractable>(Interactable))
		interactableInterface->AuthorityInteract(this);
	
}

void APlayerCharacter::PickUpItem(AWorldItem* WorldItem)
{
	if (Cast<AWorldCurrency>(WorldItem))
	{
		DestroyWorldItem(WorldItem);
		this->AddMoneyToWallet(Cast<AWorldCurrency>(WorldItem)->value);
		return;
	}
	UInventorySlot* freeSlot = FindFreeSlot();

	if (IsValid(freeSlot))
	{
		freeSlot->MyItem = WorldItem->GetMyData();
		DestroyWorldItem(WorldItem);

		if (freeSlot == GetCurrentlyHeldInventorySlot())
			TakeOutItem();
	}	
}

void APlayerCharacter::Jump()
{
	if (GetCharacterMovement()->MovementMode == MOVE_Flying)
	{
		StopClimbing();
		Super::Jump();
		return;
	}
	
	if(Stamina <= 0.f)
		return;

	SubstractStamina(JumpStaminaDrain);
	Super::Jump();
}

void APlayerCharacter::CrouchActionStarted()
{
	Cast<ADC_PC>(GetController())->GetMyPlayerHud()->UpdateCrouchIcon();
	if (!bCrouchHold)
	{
		ToggleCrouch();
		return;
	}

	Crouch(true);

}

void APlayerCharacter::CrouchActionCompleted()
{
	Cast<ADC_PC>(GetController())->GetMyPlayerHud()->UpdateCrouchIcon();
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

	Cast<ADC_PC>(GetController())->GetMyPlayerHud()->UpdateCrouchIcon();
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
	if (!this->bSprintAllowed)
		return;

	if(Stamina <= 0.f)
		return;

	if(!HasAuthority())
		Server_StartSprint();

	Server_StartSprint_Implementation();
}

void APlayerCharacter::Server_StartSprint_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed * SprintSpeedMultiplier;
	bSprinting = true;
}

void APlayerCharacter::StopSprint()
{
	if (!HasAuthority())
		Server_StopSprint();

	Server_StopSprint_Implementation();
}

void APlayerCharacter::Server_StopSprint_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
	bSprinting = false;
}

void APlayerCharacter::StartClimbingAtLocation(const FVector& Location, const FVector& InClimbUpVector)
{
	bClimbing = true;
	ClimbUpVector = InClimbUpVector;

	if (HasAuthority())
		Server_StartClimbingAtLocation_Implementation(Location, InClimbUpVector);
	else
		Server_StartClimbingAtLocation(Location, InClimbUpVector);

}

void APlayerCharacter::StopClimbing()
{	
	if(!bClimbing)
		return;

	bClimbing = false;

	OnStoppedClimbing.Broadcast();

	if (HasAuthority())
		Server_StopClimbing_Implementation();
	else
		Server_StopClimbing();

}

void APlayerCharacter::Server_StartClimbingAtLocation_Implementation(const FVector& Location, const FVector& InClimbUpVector)
{
	SetActorLocation(Location);
	GetCharacterMovement()->MovementMode = MOVE_Flying;
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	ClimbUpVector = InClimbUpVector;
}

void APlayerCharacter::Server_StopClimbing_Implementation()
{
	GetCharacterMovement()->MovementMode = MOVE_Walking;
}

void APlayerCharacter::Server_SetActorLocation_Implementation(const FVector& InLocation)
{
	SetActorLocation(InLocation);
}

void APlayerCharacter::Server_LaunchCharacter_Implementation(FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
}

void APlayerCharacter::AddStamina(float AddingStamina)
{
	if (AddingStamina < 0.f)
	{
		LogWarning("To substract stamina use \"SubstractStamina\" function");
		return;
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

	if(bSprinting)
		ToggleSprint();

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

	if(NewPlayerState)
		VOIPTalker->RegisterWithPlayerState(NewPlayerState);

}


void APlayerCharacter::AddMoneyToWallet_Implementation(float Amount)
{
	ASharedStatsManager* m= Cast<ASharedStatsManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASharedStatsManager::StaticClass()));
	m->Money += Amount;
}

void APlayerCharacter::ReportTalking(float Loudness)
{
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), Loudness, this);
}

void APlayerCharacter::Cough()
{
	if(HasAuthority())
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.0f, this);

	if(CoughSound)
		UGameplayStatics::SpawnSoundAtLocation(this, CoughSound, GetActorLocation());

}

void APlayerCharacter::ToggleInventory()
{
	this->bInventoryIsOn = !bInventoryIsOn;
	Cast<ADC_PC>(this->GetController())->GetMyPlayerHud()->ToggleInventory(bInventoryIsOn);

	auto inputLocalPlayer = GetInputLocalPlayer();

	if (!inputLocalPlayer)
		return;

	if(bInventoryIsOn)
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



UInventorySlot* APlayerCharacter::FindFreeSlot()
{
	if (bSlotAIsInHand)// a is in hand check a and then b
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

	for (UInventorySlot* S : Inventory->GetSlots())//checking for free inventory slot
	{
		if (!IsValid(S->MyItem))
			return S;
	}

	if(bHasBackPack)//checking for free backpack slots if the player has a backpack
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
	if (IsValid(CurrentlyHeldWorldItem))//destroying old item
	{
		DestroyWorldItem(CurrentlyHeldWorldItem);
	}

	UClass* newFirstPersonAnimClass = IsValid(GetCurrentlyHeldInventorySlot()->MyItem)? GetCurrentlyHeldInventorySlot()->MyItem->FirstPersonAnimationBlueprintClass : NoItemFirstPersonAnimationBlueprintClass;
	UClass* newThirdPersonAnimClass = IsValid(GetCurrentlyHeldInventorySlot()->MyItem)? GetCurrentlyHeldInventorySlot()->MyItem->ThirdPersonAnimationBlueprintClass : NoItemThirdPersonAnimationBlueprintClass;

	if (IsValid(GetCurrentlyHeldInventorySlot()->MyItem))// if its an item or just a hand
		SpawnItemInHand(GetCurrentlyHeldInventorySlot()->MyItem->MyWorldItemClass, GetCurrentlyHeldInventorySlot()->MyItem->SerializeMyData());
	
	FirstPersonMesh->SetAnimClass(newFirstPersonAnimClass);

	if(HasAuthority())
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
	if(IsLocallyControlled())
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

void APlayerCharacter::Server_SpawnItemInHand_Implementation(TSubclassOf<AWorldItem> ItemToSpawn, const FString& SerializedData)
{
	//if is in first person or not will have to make a difference

	FTransform SpawnTransform;
	CurrentlyHeldWorldItem = GetWorld()->SpawnActorDeferred<AWorldItem>(ItemToSpawn, SpawnTransform);
	CurrentlyHeldWorldItem->MyCharacterToAttachTo = this; //this property is replicated and the item will attach on begin play
	CurrentlyHeldWorldItem->SerializedStringData = SerializedData;
	CurrentlyHeldWorldItem->FinishSpawning(SpawnTransform);

}


void APlayerCharacter::DropItem(UInventorySlot* SlotToEmpty)
{
	if (IsValid(SlotToEmpty->MyItem))
	{
		SpawnDroppedWorldItem(SlotToEmpty->MyItem->MyWorldItemClass, SlotToEmpty->MyItem->SerializeMyData());
		SlotToEmpty->MyItem = nullptr;

		if (GetCurrentlyHeldInventorySlot() == SlotToEmpty)
		{
			TakeOutItem();
		}

	}
}

void APlayerCharacter::RemoveInventorySlot(UInventorySlot* SlotToEmpty)
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
		DropItem(pc->GetMyPlayerHud()->GetHighlightedSlot());
		return;
	}

	if (!bSwitchHandAllowed || bInventoryIsOn)
		return;
	
	bSwitchHandAllowed = false;

	this->bSlotAIsInHand = !bSlotAIsInHand;
	TakeOutItem();

	Cast<ADC_PC>(GetController())->GetMyPlayerHud()->OnSwichingDone.AddDynamic(this, &APlayerCharacter::AllowSwitchHand);
	Cast<ADC_PC>(GetController())->GetMyPlayerHud()->SwichHandDisplays(bSlotAIsInHand);
	
}

void APlayerCharacter::AllowSwitchHand()
{
	bSwitchHandAllowed = true;
	Cast<ADC_PC>(GetController())->GetMyPlayerHud()->OnSwichingDone.RemoveAll(this);
}

void APlayerCharacter::EquipCurrentInventorySelection(bool BToA)
{
	
	UInventorySlot* slot;

	if (BToA)
		slot = HandSlotA;
	else
		slot = HandSlotB;
	
	//switch
	UItemData* tmp = Cast<ADC_PC>(GetController())->GetMyPlayerHud()->GetHighlightedSlot()->MyItem;
	Cast<ADC_PC>(GetController())->GetMyPlayerHud()->GetHighlightedSlot()->MyItem = slot->MyItem;
	slot->MyItem = tmp;


	if (GetCurrentlyHeldInventorySlot()==slot)//if equipping to slot in hand
	{
		TakeOutItem();
	}

}

void APlayerCharacter::DropItemPressed()
{
	DropItem(GetCurrentlyHeldInventorySlot());
}

void APlayerCharacter::TriggerPrimaryItemAction()
{
	if(bInventoryIsOn)
		return;

	if (HasAuthority())
	{
		Server_TriggerPrimaryItemAction_Implementation();
		return;
	}

	Server_TriggerPrimaryItemAction();
}

void APlayerCharacter::TriggerSecondaryItemAction()
{
	if (bInventoryIsOn)
		return;

	if (HasAuthority())
	{
		Server_TriggerSecondaryItemAction_Implementation();
		return;
	}

	Server_TriggerSecondaryItemAction();
}

void APlayerCharacter::SpawnDroppedWorldItem(TSubclassOf<AWorldItem> ItemToSpawn, const FString& SerializedData)
{
	if (!HasAuthority())
		Server_SpawnDroppedWorldItem(ItemToSpawn,SerializedData);
	else
		Server_SpawnDroppedWorldItem_Implementation(ItemToSpawn, SerializedData);
}

void APlayerCharacter::Server_SpawnDroppedWorldItem_Implementation(TSubclassOf<AWorldItem> ItemToSpawn, const FString& SerializedData)
{
	LogWarning(TEXT("Spawning Item"));
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(this->FirstPersonCamera->GetComponentLocation() + this->FirstPersonCamera->GetForwardVector() * 150 - FVector(0, 0, 20));

	AWorldItem* i = GetWorld()->SpawnActorDeferred<AWorldItem>(ItemToSpawn, SpawnTransform);
	i->SerializedStringData = SerializedData;
	i->FinishSpawning(SpawnTransform);

	//GetWorld()->SpawnActor<AWorldItem>(ItemToSpawn, SpawnTransform);
	//SpawnedItem->GetRootComponent()->AddImpulse()
}



void APlayerCharacter::CheckForFallDamage()
{
	// i am using Velocity.z instead of movementComponent::IsFalling() because it already counts as falling when the player is in the air while jumping. 
	// that results in the jump height not being included in the fall height calculation

	if (GetMovementComponent()->Velocity.Z==0 && BWasFallingInLastFrame && GetCharacterMovement()->MovementMode != MOVE_Flying)//frame of impact
	{
		float deltaZ = LastStandingHeight - this->RootComponent->GetComponentLocation().Z+20;//+20 artificially because the capsule curvature lets the player stand lower
		if (deltaZ > 200)
		{
			
			float damage = this->FallDamageCalculation(deltaZ);
			TakeDamage(damage);
		}

		//FString message = 
		//	"\n\nStart height:\t"+FString::SanitizeFloat(LastStandingHeight)+
		//	"\nEnd height:\t"+FString::SanitizeFloat(RootComponent->GetComponentLocation().Z)
		//	+ "\nFall height:\t " + FString::SanitizeFloat(deltaZ);
		//LogWarning(*message);
	}
	if (GetMovementComponent()->Velocity.Z >= 0 || GetCharacterMovement()->MovementMode == MOVE_Flying)
		LastStandingHeight = this->RootComponent->GetComponentLocation().Z;

	this->BWasFallingInLastFrame = (GetMovementComponent()->Velocity.Z < 0 && GetCharacterMovement()->MovementMode != MOVE_Flying);
}


float APlayerCharacter::FallDamageCalculation(float deltaHeight)
{

	float free = 300;
	float max = 1000;

	if (deltaHeight <= free)
		return 0;


	float factor = 100 / (max - free);

	return (deltaHeight- free) * factor;
}

void APlayerCharacter::NavigateInventory(const FInputActionValue& Value)
{
	FVector2D input = Value.Get<FVector2D>();

	ADC_PC* pc = GetController<ADC_PC>();

	if(!pc || !pc->GetMyPlayerHud())
		return;

	pc->GetMyPlayerHud()->MoveHighlight(EDirections::Down);

	int x = 5;

	if (input.Y != 0)
	{
		pc->GetMyPlayerHud()->MoveHighlight(input.Y > 0? EDirections::Up : EDirections::Down);
	}
	
	if (input.X != 0)
	{
		pc->GetMyPlayerHud()->MoveHighlight(input.X > 0 ? EDirections::Right : EDirections::Left);
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

	DropItem(pc->GetMyPlayerHud()->GetHighlightedSlot());
}

void APlayerCharacter::OnDeath_Implementation()
{
	Super::OnDeath_Implementation();

	GetCapsuleComponent()->SetCollisionProfileName("DeadPawn", true);
	StimulusSource->DestroyComponent(false);

	if (!HasAuthority())
		return;

	GetWorld()->GetAuthGameMode<ADC_GM>()->Respawn(GetController());

}


void APlayerCharacter::Server_TriggerPrimaryItemAction_Implementation()
{
	if (!bPrimaryActionAllowed || !IsValid(CurrentlyHeldWorldItem))
		return;

	CurrentlyHeldWorldItem->TriggerPrimaryAction(this);
}

void APlayerCharacter::Server_TriggerSecondaryItemAction_Implementation()
{
	if (!bSecondaryActionAllowed || !IsValid(CurrentlyHeldWorldItem))
		return;

	CurrentlyHeldWorldItem->TriggerSecondaryAction(this);
}



void APlayerCharacter::StartAttacking()
{
	AttackStart();

	if (!HasAuthority())
		Server_AttackStart();
	else
		Multicast_AttackStart();
}

void APlayerCharacter::AttackStart()
{

	//different attack when sprinting?
	//attack needs to cost stamina
	this->bPrimaryActionAllowed = false;
	this->AttackBlend = 1;
	this->bSwitchHandAllowed = false;
	//this->bMoveAllowed = false;
	//this->bLookAllowed = false;


	this->bSprintAllowed = false;
	GetCharacterMovement()->MaxWalkSpeed = 100;

	if(IsLocallyControlled())
		StopSprint();
	
}

void APlayerCharacter::Server_AttackStart_Implementation()
{
	AttackStart();
}

void APlayerCharacter::Multicast_AttackStart_Implementation()
{
	if(IsLocallyControlled())
		return;

	AttackStart();

}

void APlayerCharacter::AttackLanded()
{
	AWeapon* weapon = Cast<AWeapon>(CurrentlyHeldWorldItem);
	
	weapon->DealHits(NULL, FVector(), FVector());
}

void APlayerCharacter::OnAttackOver()
{
	this->AttackBlend = 0;
	this->bSwitchHandAllowed = true;
	//this->bMoveAllowed = true;
	//this->bLookAllowed = true;
	this->bPrimaryActionAllowed = true;
	//allow sprint
	this->bSprintAllowed = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
	
}
