// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/Ladder.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "PlayerCharacter/Components/DC_CMC.h"

void ALadder::SetHeight(float InHeight)
{
	Height = InHeight;
}

// Sets default values
ALadder::ALadder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bInteractable = false;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	BottomBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BottomBox"));
	BottomBox->SetupAttachment(RootComponent);

	BottomBox->InitBoxExtent(FVector(100, 100, 5));
	BottomBox->SetRelativeLocation(BottomBox->GetUnscaledBoxExtent().Z * FVector::UpVector);

	InteractVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractVolume->SetupAttachment(RootComponent);

	InteractVolume->InitBoxExtent(FVector(1, 1, 1));

	ClimbVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ClimbVolume"));
	ClimbVolume->SetupAttachment(RootComponent);

	ClimbVolume->InitBoxExtent(FVector(1, 1, 1));

	EasyInteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("EasyInteractBox"));
	EasyInteractBox->SetupAttachment(RootComponent);

	EasyInteractBox->SetCollisionProfileName(FName("EasyInteract"));

	EasyInteractBox->InitBoxExtent(FVector(1, 1, 1));
}

void ALadder::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	float iVHalfHeight = Height / 2 + InteractionVolumeHeightBonus / 2;

	InteractVolume->SetBoxExtent(FVector(InteractionArea, iVHalfHeight));
	InteractVolume->SetRelativeLocation(FVector(InteractionArea.X, 0, iVHalfHeight));

	float halfHeight = Height / 2;

	ClimbVolume->SetBoxExtent(FVector({25, 5}, halfHeight));
	ClimbVolume->SetRelativeLocation(FVector(25, 0, halfHeight));

	EasyInteractBox->SetBoxExtent(FVector(EasyInteractArea, halfHeight));
	EasyInteractBox->SetRelativeLocation(FVector(0, 0, halfHeight));

}

// Called when the game starts or when spawned
void ALadder::BeginPlay()
{
	Super::BeginPlay();

	BottomBox->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnBottomBoxOverlap);

	InteractVolume->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnInteractVolumeEntered);
	InteractVolume->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnInteractVolumeLeft);

	ClimbVolume->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnClimbVolumeLeft);
	
}

void ALadder::Interact(APawn* InteractingPawn)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(InteractingPawn);
	
	if(!character)
		return;

	float distanceToLadder = character->GetCapsuleComponent()->GetScaledCapsuleRadius();

	float distanceToStart = character->GetDistanceTo(this);

	if(distanceToStart > Height)
		distanceToStart = Height;

	FVector climbPosition = GetActorLocation() + GetActorUpVector() * distanceToStart + GetActorForwardVector() * distanceToLadder;
	bInteractable = false;

	character->StartClimbingAtLocation(climbPosition, GetActorUpVector());

	LocalPlayerOnLadder = character;
	character->OnStoppedClimbing.AddDynamic(this, &ALadder::StoppedInteracting);
	bRemovedByLadder = false;
}

void ALadder::OnBottomBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);
	if (!character || !character->IsLocallyControlled())
		return;

	bRemovedByLadder = true;

	character->StopClimbing();
	
}

void ALadder::OnInteractVolumeEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);
	if(!character || !character->IsLocallyControlled())
		return;

	bInteractable = true;

}

void ALadder::OnInteractVolumeLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);
	if (!character || !character->IsLocallyControlled())
		return;

	bInteractable = false;

}

void ALadder::OnClimbVolumeLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);
	if (!character || !character->IsLocallyControlled())
		return;

	bRemovedByLadder = true;

	character->StopClimbing();
}

void ALadder::StoppedInteracting()
{
	bInteractable = true;
	if(!IsValid(LocalPlayerOnLadder))
		return;

	LocalPlayerOnLadder->OnStoppedClimbing.RemoveAll(this);
	APlayerCharacter* LaunchCharacter = LocalPlayerOnLadder;
	LocalPlayerOnLadder = nullptr;

	if(bRemovedByLadder)
		return;

	FVector launchDirection = GetActorForwardVector() + FVector::UpVector * 3;
	launchDirection.Normalize();

	FVector launchVelocity = launchDirection * LaunchCharacter->GetCharacterMovement()->JumpZVelocity;
	LaunchCharacter->LaunchCharacter(launchVelocity, false, true);

	if(!HasAuthority())
		LaunchCharacter->Server_LaunchCharacter(launchVelocity, false, true);

}

