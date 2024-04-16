// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter/PlayerCharacter.h"
#include "PlayerCharacter/DungeonCompanyPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	bUseControllerRotationYaw = false;//nessecary?

	this->firstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	this->firstPersonCamera->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	this->firstPersonCamera->SetRelativeLocation(FVector(0, 0, 40));


	this->FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	this->FirstPersonMesh->AttachToComponent(this->firstPersonCamera, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	this->WalkingSpeed = 1;

	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;

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
	
	this->ApplyMovement(this->movementVector);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("Forward", this, &APlayerCharacter::VericalMovement);
	PlayerInputComponent->BindAxis("Right",this, &APlayerCharacter::HorizontalMovement);
	
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

void APlayerCharacter::VericalMovement(float value)
{
	this->movementVector.X = value;
}

void APlayerCharacter::ApplyMovement(FVector v)// to resolve faster diagonal movement
{
	if (v.Length() > 1)
	{
		v.Normalize(0.001);
	}

	v *= WalkingSpeed;
	AddMovementInput(GetActorForwardVector()*v.X);
	AddMovementInput(GetActorRightVector()*v.Y);
	
}
