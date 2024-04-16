// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter/PlayerCharacter.h"
#include "PlayerCharacter/DungeonCompanyPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FirstPersonCamera->SetupAttachment(RootComponent);
	FirstPersonCamera->SetRelativeLocation(FVector(0, 0, 40));
	FirstPersonCamera->bUsePawnControlRotation = true;
		   

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);

	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, -1.0f, 0.0f);
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Forward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Right",this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MouseRight",this, &ACharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("MouseUp",this, &ACharacter::AddControllerPitchInput);

	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}

void APlayerCharacter::HorizontalMovement(float value)
{
	this->movementVector.Y = value;
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
