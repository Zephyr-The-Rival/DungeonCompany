// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter/PlayerCharacter.h"
#include "PlayerCharacter/Components/DC_CMC.h"
#include "DCGame/DC_PC.h"
#include "DC_Statics.h"
#include "Interactable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/VoiceConfig.h"

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

	GetCharacterMovement()->MaxWalkSpeed = this->WalkingSpeed;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, -1.0f, 0.0f);
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(60.f);

	static ConstructorHelpers::FObjectFinder<USoundAttenuation> voiceSA(TEXT("/Game/_DungeonCompanyContent/Audio/Player/VoiceSA.VoiceSA"));
	VoiceSA = voiceSA.Object;

	VOIPTalker = CreateDefaultSubobject<UVOIPTalker>(TEXT("VOIPTalker"));


}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	VOIPTalker->Settings.AttenuationSettings = VoiceSA;
	VOIPTalker->Settings.ComponentToAttachTo = FirstPersonCamera;
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	this->InteractorLineTrace();

	

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &APlayerCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &APlayerCharacter::ToggleCrouch);
	PlayerInputComponent->BindAxis("Forward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Right",this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MouseRight",this, &ACharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("MouseUp",this, &ACharacter::AddControllerPitchInput);

}

void APlayerCharacter::MoveRight(float Value)
{
	Move(GetActorRightVector() * Value);

}

void APlayerCharacter::MoveForward(float Value)
{
	Move(GetActorForwardVector() * Value);

}

void APlayerCharacter::Move(FVector MoveVector)
{
	AddMovementInput(MoveVector);

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
		if (i)
		{
			LogWarning(*("interactable " + Hit.GetActor()->GetName() + " was hit"));
		}

	}
}

void APlayerCharacter::ToggleCrouch()
{
	if (GetCharacterMovement()->IsCrouching())
		UnCrouch(true);
	else
		Crouch(true);

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
