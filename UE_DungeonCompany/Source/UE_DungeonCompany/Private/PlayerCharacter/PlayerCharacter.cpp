// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter/PlayerCharacter.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationYaw = false;//nessecary?

	this->camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	this->camera->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	camera->SetRelativeLocation(FVector(0, 0, 40));

	this->WalkingSpeed = 1;



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
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Forward", this, &APlayerCharacter::VericalMovement);
	InputComponent->BindAxis("Right",this, &APlayerCharacter::HorizontalMovement);
	
	InputComponent->BindAxis("MouseRight",this, &APlayerCharacter::HorizontalRotaion);
	InputComponent->BindAxis("MouseUp",this, &APlayerCharacter::VerticalRotation);

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
	v.Normalize(0.001);
	UE_LOG(LogTemp, Log, TEXT("MovementVector: %s"), *v.ToString());
	v *= WalkingSpeed;
	AddMovementInput(GetActorForwardVector()*v.X);
	AddMovementInput(GetActorRightVector()*v.Y);
	
}

void APlayerCharacter::HorizontalRotaion(float value)
{
	RootComponent->AddLocalRotation(FRotator(0,value,0));
}

void APlayerCharacter::VerticalRotation(float value)
{
	float temp = camera->GetRelativeRotation().Pitch + value;

	if (temp<90 && temp>-90)
	{
		camera->AddLocalRotation(FRotator(value, 0, 0));
	}
}



