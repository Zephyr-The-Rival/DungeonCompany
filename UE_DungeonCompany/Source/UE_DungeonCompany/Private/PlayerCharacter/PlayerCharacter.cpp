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
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"


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
#include "Online.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
// Sets default values
APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDC_CMC>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FirstPersonCamera->SetupAttachment(RootComponent);
	FirstPersonCamera->SetRelativeLocation(FVector(0, 0, 40));
	FirstPersonCamera->bUsePawnControlRotation = true;
		   

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);

	GetCharacterMovement()->BrakingDecelerationFlying = 5000.f;
	GetCharacterMovement()->MaxWalkSpeed = this->WalkingSpeed;
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

	if(!InputMapping)
		return;

	APlayerController* playerController = GetController<APlayerController>();

	if(!playerController)
		return;

	ULocalPlayer* localPlayer = playerController->GetLocalPlayer();

	if(!localPlayer)
		return;

	UEnhancedInputLocalPlayerSubsystem* inputSystem = playerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if(!inputSystem)
		return;
	
	inputSystem->AddMappingContext(InputMapping, 0);
	

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float voiceLevel = VOIPTalker->GetVoiceLevel();

	if(voiceLevel > 0.f)
		ReportTalking(voiceLevel);
	
	if(IsLocallyControlled())
		LocalTick(DeltaTime);

}

void APlayerCharacter::LocalTick(float DeltaTime)
{
	this->InteractorLineTrace();
	StaminaTick(DeltaTime);
	IOnlineVoicePtr ptr = Online::GetVoiceInterface(IOnlineSubsystem::Get()->GetSubsystemName());
	CheckForFallDamage();
}

void APlayerCharacter::StaminaTick(float DeltaTime)
{
	if (bResting)
		AddStamina(StaminaGainPerSecond * DeltaTime);

	if (!bSprinting)
		return;

	if (GetCharacterMovement()->Velocity.Length() > WalkingSpeed && GetCharacterMovement()->IsMovingOnGround())
			SubstractStamina(SprintStaminaDrainPerSecond * DeltaTime);

	if (Stamina < 0.f)
		ToggleSprint();
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

	EIC->BindAction(DPadUpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::DPadUpPressed);
	EIC->BindAction(DPadDownAction, ETriggerEvent::Triggered, this, &APlayerCharacter::DPadDownPressed);
	EIC->BindAction(DPadLeftAction, ETriggerEvent::Triggered, this, &APlayerCharacter::DPadLeftPressed);
	EIC->BindAction(DPadRightAction, ETriggerEvent::Triggered, this, &APlayerCharacter::DPadRightPressed);
	
	EIC->BindAction(ToggleInventoryPCAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ToggleInventoryPC);
	EIC->BindAction(ToggleInventoryControllerAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ToggleInventoryController);
	
	EIC->BindAction(FaceUpAction,		ETriggerEvent::Triggered, this, &APlayerCharacter::FaceUpPressed);	
	EIC->BindAction(FaceDownAction,		ETriggerEvent::Triggered, this, &APlayerCharacter::FaceDownPressed);
	EIC->BindAction(FaceLeftAction,		ETriggerEvent::Triggered, this, &APlayerCharacter::FaceLeftPressed);
	EIC->BindAction(FaceRightAction,	ETriggerEvent::Triggered, this, &APlayerCharacter::FaceRightPressed);
	
	EIC->BindAction(MouseRightAction,	ETriggerEvent::Triggered, this, &APlayerCharacter::RightMouseButtonPressed);
	EIC->BindAction(MouseLeftAction,	ETriggerEvent::Triggered, this, &APlayerCharacter::LeftMouseButtonPressed);
	
	EIC->BindAction(ScrollAction,	ETriggerEvent::Triggered, this, &APlayerCharacter::MouseWheelScrolled);
	EIC->BindAction(DropItemAction,	ETriggerEvent::Triggered, this, &APlayerCharacter::DropItemPressed);

	

	


}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
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
	FVector2D lookVector = Value.Get<FVector2D>();

	AddControllerYawInput(lookVector.X);
	AddControllerPitchInput(lookVector.Y);

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
	UInventorySlot* freeSlot = FindFreeSlot();

	if (IsValid(freeSlot))
	{
		freeSlot->MyItem = WorldItem->MyData;
		DestroyWorldItem(WorldItem);

		if (freeSlot == GetCurrentlyHeldInventorySlot())
			TakeOutItem();
	}	
}

void APlayerCharacter::Jump()
{
	if (GetCharacterMovement()->MovementMode == MOVE_Flying)
		StopClimbing();

	Super::Jump();

}

void APlayerCharacter::CrouchActionStarted()
{
	if (!bCrouchHold)
	{
		ToggleCrouch();
		return;
	}

	Crouch(true);

}

void APlayerCharacter::CrouchActionCompleted()
{
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

void APlayerCharacter::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);

	if(NewPlayerState)
		VOIPTalker->RegisterWithPlayerState(NewPlayerState);

}

void APlayerCharacter::ReportTalking(float Loudness)
{
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), Loudness, this);
}

void APlayerCharacter::ToggleInventoryPC()
{
	ToggleInventory(false);
}

void APlayerCharacter::ToggleInventoryController()
{
	ToggleInventory(true);
}

void APlayerCharacter::ToggleInventory(bool ControllerVersion)
{
	this->BInventoryIsOn = !BInventoryIsOn;
	Cast<ADC_PC>(this->GetController())->GetMyPlayerHud()->ToggleInventory(BInventoryIsOn, ControllerVersion);
}



UInventorySlot* APlayerCharacter::GetCurrentlyHeldInventorySlot()
{
	if (this->BSlotAIsInHand)
		return HandSlotA;
	else
		return HandSlotB;
}



UInventorySlot* APlayerCharacter::FindFreeSlot()
{
	if (BSlotAIsInHand)// a is in hand check a and then b
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

	if(this->BHasBackPack)//checking for free backpack slots if the player has a backpack
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
		CurrentlyHeldWorldItem->Destroy();

	if (IsValid(GetCurrentlyHeldInventorySlot()->MyItem))
	{
		this->FirstPersonMesh->SetAnimClass(GetCurrentlyHeldInventorySlot()->MyItem->AnimationBlueprintClass);
		SpawnItemInHand(GetCurrentlyHeldInventorySlot()->MyItem->MyWorldItemClass);
	}
	else
	{
		//animClass of regular hands
	}
}



void APlayerCharacter::SpawnItemInHand(TSubclassOf<AWorldItem> ItemToSpawn)
{
	if (HasAuthority())
		Server_SpawnItemInHand_Implementation(ItemToSpawn);
	else
		Server_SpawnItemInHand(ItemToSpawn);
}

void APlayerCharacter::Server_SpawnItemInHand_Implementation(TSubclassOf<AWorldItem> ItemToSpawn)
{
	//if is in first person or not will have to make a difference


	FTransform SpawnTransform;
	CurrentlyHeldWorldItem = GetWorld()->SpawnActor<AWorldItem>(ItemToSpawn, SpawnTransform);

	AttachItemToHand();
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, CurrentlyHeldWorldItem);
}


void APlayerCharacter::AttachItemToHand_Implementation()
{

	CurrentlyHeldWorldItem->OnHoldingInHand();
	CurrentlyHeldWorldItem->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true), "Item_Joint_R");
}

void APlayerCharacter::DropItem(UInventorySlot* SlotToEmpty)
{
	if (IsValid(SlotToEmpty->MyItem))
	{
		SpawnDroppedWorldItem(SlotToEmpty->MyItem->MyWorldItemClass);
		SlotToEmpty->MyItem = nullptr;

		if (GetCurrentlyHeldInventorySlot()==SlotToEmpty)
		{
			CurrentlyHeldWorldItem->Destroy();
			TakeOutItem();
		}

	}
}

void APlayerCharacter::SwitchHand()
{
	if (BSwichHandAllowed)
	{
		BSwichHandAllowed = false;
		this->BSlotAIsInHand = !BSlotAIsInHand;
		TakeOutItem();
		Cast<ADC_PC>(GetController())->GetMyPlayerHud()->OnSwichingDone.AddDynamic(this, &APlayerCharacter::AllowSwitchHand);
		Cast<ADC_PC>(GetController())->GetMyPlayerHud()->SwichHandDisplays(BSlotAIsInHand);
	}
}

void APlayerCharacter::AllowSwitchHand()
{
	BSwichHandAllowed = true;
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
	if (this->BInventoryIsOn)
		DropItem(Cast<ADC_PC>(GetController())->GetMyPlayerHud()->GetHighlightedSlot());
	else
		DropItem(GetCurrentlyHeldInventorySlot());
}

void APlayerCharacter::SpawnDroppedWorldItem(TSubclassOf<AWorldItem> ItemToSpawn)
{
	if (!HasAuthority())
		Server_SpawnDroppedWorldItem(ItemToSpawn);
	else
	Server_SpawnDroppedWorldItem_Implementation(ItemToSpawn);
}

void APlayerCharacter::Server_SpawnDroppedWorldItem_Implementation(TSubclassOf<AWorldItem> ItemToSpawn)
{
	LogWarning(TEXT("Spawning Item"));
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(this->FirstPersonCamera->GetComponentLocation() + this->FirstPersonCamera->GetForwardVector() * 150 - FVector(0, 0, 20));

	GetWorld()->SpawnActor<AWorldItem>(ItemToSpawn, SpawnTransform);
	//SpawnedItem->GetRootComponent()->AddImpulse()
	//set item data
}

void APlayerCharacter::TakeDamage_DC(float amout)
{
	FString message = "Taking damage: " + FString::SanitizeFloat(amout);
	LogWarning(*message);
	if (this->HP - amout > 0)
	{
		HP -= amout;
	}
	else
	{
		HP = 0;
		Cast<ADC_PC>(GetController())->ConsoleCommand("Quit");
	}
}

void APlayerCharacter::CheckForFallDamage()
{
	// i am using Velocity.z instead of movementComponent::IsFalling() because it already counts as falling when the player is in the air while jumping. 
	// that results in the jump height not being included in the fall height calculation

	if (GetMovementComponent()->Velocity.Z==0 && BWasFallingInLastFrame)//frame of impact
	{
		float deltaZ = LastStandingHeight - this->RootComponent->GetComponentLocation().Z+20;//+20 artificially because the capsule curvature lets the player stand lower
		if (deltaZ > 200)
		{
			float damage = (deltaZ - 200) * 0.334; // 2m=0 damage 5m=100 dmg
			this->TakeDamage_DC(damage);
		}
		//FString message = 
		//	"\n\nStart height:\t"+FString::SanitizeFloat(LastStandingHeight)+
		//	"\nEnd height:\t"+FString::SanitizeFloat(RootComponent->GetComponentLocation().Z)
		//	+ "\nFall height:\t " + FString::SanitizeFloat(deltaZ);
		//LogWarning(*message);
	}
	if (GetMovementComponent()->Velocity.Z>=0)
		LastStandingHeight = this->RootComponent->GetComponentLocation().Z;

	this->BWasFallingInLastFrame = (GetMovementComponent()->Velocity.Z < 0);
}

void APlayerCharacter::DPadUpPressed()
{
	if (BInventoryIsOn)
	{
		Cast<ADC_PC>(GetController())->GetMyPlayerHud()->MoveHighlight(EDirections::Up);
	}
	else
	{
		//throw
	}
}

void APlayerCharacter::DPadDownPressed()
{
	if (BInventoryIsOn)
	{
		Cast<ADC_PC>(GetController())->GetMyPlayerHud()->MoveHighlight(EDirections::Down);
	}
	else
	{
		DropItem(GetCurrentlyHeldInventorySlot());
	}
}

void APlayerCharacter::DPadLeftPressed()
{
	if (BInventoryIsOn)
	{
		Cast<ADC_PC>(GetController())->GetMyPlayerHud()->MoveHighlight(EDirections::Left);
	}
	else
	{
		
	}
}

void APlayerCharacter::DPadRightPressed()
{
	if (BInventoryIsOn)
	{
		Cast<ADC_PC>(GetController())->GetMyPlayerHud()->MoveHighlight(EDirections::Right);
	}
	else
	{
		
	}
}

void APlayerCharacter::FaceUpPressed()
{
	if (BInventoryIsOn)
	{
		DropItem(Cast<ADC_PC>(GetController())->GetMyPlayerHud()->GetHighlightedSlot());
	}
	else
	{
		this->SwitchHand();
	}
}

void APlayerCharacter::FaceDownPressed()
{
	if (BInventoryIsOn)
	{

	}
	else
	{
		Jump();
	}
}

void APlayerCharacter::FaceLeftPressed()
{
	if (BInventoryIsOn)
	{
		EquipCurrentInventorySelection(true);
	}
	else
	{
		Interact();
	}
}

void APlayerCharacter::FaceRightPressed()
{
	if (BInventoryIsOn)
	{
		EquipCurrentInventorySelection(false);
	}
	else
	{
		ToggleCrouch();
	}
}

void APlayerCharacter::LeftMouseButtonPressed()
{
	if (BInventoryIsOn)
	{
		EquipCurrentInventorySelection(true);
	}
	else
	{
		
	}
}

void APlayerCharacter::RightMouseButtonPressed()
{
	if (BInventoryIsOn)
	{
		EquipCurrentInventorySelection(false);


	}
	else
	{
		
	}
}

void APlayerCharacter::MouseWheelScrolled(const FInputActionValue& Value)
{
	if (BInventoryIsOn)
	{
		Cast<ADC_PC>(GetController())->GetMyPlayerHud()->MoveHighlightScroll((Value.Get<float>() > 0));
	}
	else
	{
		SwitchHand();
	}

}

