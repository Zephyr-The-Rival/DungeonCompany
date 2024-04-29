// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter/PlayerCharacter.h"
#include "PlayerCharacter/Components/DC_CMC.h"
#include "DCGame/DC_PC.h"
#include "DC_Statics.h"
#include "UI/PlayerHud/PlayerHud.h"
#include "Items/WorldItem.h"
#include "Items/ItemData.h"
#include "Inventory/Inventory.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/VoiceConfig.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"

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
	this->InventoryIndexInFocus = 0;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();


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
	EIC->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Jump);
	EIC->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::CrouchActionStarted);
	EIC->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerCharacter::CrouchActionCompleted);
	EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::SprintActionStarted);
	EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::SprintActionCompleted);

	EIC->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);

	EIC->BindAction(IterateItemsLeftAction, ETriggerEvent::Triggered, this, &APlayerCharacter::IterateItemsLeft);
	EIC->BindAction(IterateItemsRightAction, ETriggerEvent::Triggered, this, &APlayerCharacter::IterateItemsRight);
	
	EIC->BindAction(DropItemAction, ETriggerEvent::Triggered, this, &APlayerCharacter::DropItem);
	


}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D localMoveVector = Value.Get<FVector2D>();

	FVector worldMoveVector;

	if (GetCharacterMovement()->MovementMode != MOVE_Flying)
		worldMoveVector = GetActorRightVector() * localMoveVector.X + GetActorForwardVector() * localMoveVector.Y;
	else
		worldMoveVector = FVector::UpVector * localMoveVector.Y;

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
	AddControllerPitchInput(-lookVector.Y);

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
	LogWarning(TEXT("SERVER DESTROY CALLED"));
	ItemToDestroy->Destroy();
}

void APlayerCharacter::Interact()
{
	if(!CurrentInteractable)
		return;

	CurrentInteractable->Interact(this);

}

void APlayerCharacter::PickUpItem(AWorldItem* WorldItem)
{
	if (this->Inventory->AddItem(WorldItem->MyData))
	{
		FString message = WorldItem->MyData->Name + " has been picked up";
		LogWarning(*message);

		DestroyWorldItem(WorldItem);
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

void APlayerCharacter::StartClimbingAtLocation(const FVector& Location)
{
	bClimbing = true;

	if (HasAuthority())
		Server_StartClimbingAtLocation_Implementation(Location);
	else
		Server_StartClimbingAtLocation(Location);

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

void APlayerCharacter::Server_StartClimbingAtLocation_Implementation(const FVector& Location)
{
	SetActorLocation(Location);
	GetCharacterMovement()->MovementMode = MOVE_Flying;
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
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

void APlayerCharacter::IterateItemsLeft()
{
	if (InventoryIndexInFocus == 0)
		InventoryIndexInFocus = this->Inventory->NumInventorySlots - 1;
	else
		this->InventoryIndexInFocus--;

	Cast<ADC_PC>(this->GetController())->GetMyPlayerHud()->FocusOnInventorySlot(this->InventoryIndexInFocus);
}

void APlayerCharacter::IterateItemsRight()
{
	if (InventoryIndexInFocus == this->Inventory->NumInventorySlots - 1)
		InventoryIndexInFocus = 0;
	else
		this->InventoryIndexInFocus++;

	Cast<ADC_PC>(this->GetController())->GetMyPlayerHud()->FocusOnInventorySlot(this->InventoryIndexInFocus);

}

void APlayerCharacter::DropItem()
{
	if(IsValid(this->Inventory->GetItemAtIndex(InventoryIndexInFocus)))
	SpawnDroppedWorldItem(this->Inventory->GetItemAtIndex(InventoryIndexInFocus)->MyWorldItem);	
}

void APlayerCharacter::SpawnDroppedWorldItem(TSubclassOf<AWorldItem> ItemToSpawn)
{
	if (!HasAuthority())
		Server_SpawnDroppedWorldItem(ItemToSpawn);
	
	Server_SpawnDroppedWorldItem_Implementation(ItemToSpawn);
}

void APlayerCharacter::Server_SpawnDroppedWorldItem_Implementation(TSubclassOf<AWorldItem> ItemToSpawn)
{
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(this->FirstPersonCamera->GetComponentLocation() + this->FirstPersonCamera->GetForwardVector() * 20 - FVector(0, 0, 20));
	AWorldItem* SpawnedItem = GetWorld()->SpawnActor<AWorldItem>(*ItemToSpawn, SpawnTransform);
	//SpawnedItem->GetRootComponent()->AddImpulse()
}


