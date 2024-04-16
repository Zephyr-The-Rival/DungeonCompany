// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter/PlayerCharacter.h"
#include "PlayerCharacter/DungeonCompanyPlayerController.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/VoiceConfig.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	static ConstructorHelpers::FObjectFinder<USoundAttenuation> voiceSA(TEXT("/Game/_DungeonCompanyContent/Audio/Player/VoiceSA.VoiceSA"));

	VoiceSA = voiceSA.Object;

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

	VOIPTalker = CreateDefaultSubobject<UVOIPTalker>(TEXT("VOIPTalker"));

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupVOIPTalker();
	
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

}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	auto DCPC = Cast<ADungeonCompanyPlayerController>(NewController);

	if(!DCPC)
		return;

	if(!HasAuthority())
		return;

	//DCPC->AttachVOIPTalkerTo(GetRootComponent());

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

void APlayerCharacter::SetupVOIPTalker()
{
	APlayerState* playerState = GetPlayerState<APlayerState>();

	if (!playerState)
	{
		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &APlayerCharacter::SetupVOIPTalker, 0.1f);
		return;
	}

	VOIPTalker->RegisterWithPlayerState(playerState);

	VOIPTalker->Settings.AttenuationSettings = VoiceSA;
	VOIPTalker->Settings.ComponentToAttachTo = FirstPersonCamera;

	if (!IsLocallyControlled())
		return;

	UVOIPStatics::SetMicThreshold(-1.0);

	GetWorld()->Exec(GetWorld(), TEXT("OSS.VoiceLoopback 1"));
	GetController<APlayerController>()->ToggleSpeaking(true);
}
