// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/Ladder.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "PlayerCharacter/Components/DC_CMC.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

void ALadder::SetHeight(float InHeight)
{
	Height = InHeight;
}

void ALadder::SetIgnoreInteractionVolume(bool InIgnore)
{
	bIgnoreInteractionVolume = InIgnore;

	InteractVolume->OnComponentBeginOverlap.RemoveAll(this);
	InteractVolume->OnComponentEndOverlap.RemoveAll(this);

	if (!bIgnoreInteractionVolume)
	{
		InteractVolume->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnInteractVolumeEntered);
		InteractVolume->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnInteractVolumeLeft);
		return;
	}

	if (!IsValid(LocalPlayerOnLadder))
	{
		bInteractable = true;
	}

}

// Sets default values
ALadder::ALadder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bInteractable = false;
	bInteractOnServer = false;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	InteractVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractVolume->SetupAttachment(RootComponent);

	InteractVolume->InitBoxExtent(FVector(1, 1, 1));

	ClimbVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ClimbVolume"));
	ClimbVolume->SetupAttachment(RootComponent);

	ClimbVolume->InitBoxExtent(FVector(1, 1, 1));

	EasyInteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("EasyInteractBox"));
	EasyInteractBox->SetupAttachment(RootComponent);

	EasyInteractBox->InitBoxExtent(FVector(1, 1, 1));
}

void ALadder::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	CalculateLadderBoxExtents();

}

// Called when the game starts or when spawned
void ALadder::BeginPlay()
{
	Super::BeginPlay();

	if (!bIgnoreInteractionVolume)
	{
		InteractVolume->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnInteractVolumeEntered);
		InteractVolume->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnInteractVolumeLeft);
	}
	
}

void ALadder::CalculateLadderBoxExtents()
{
	float iVHalfHeight = Height / 2 + InteractionVolumeHeightBonus / 2;
	
	InteractVolume->SetBoxExtent(FVector(InteractionArea, iVHalfHeight));
	InteractVolume->SetRelativeLocation(FVector(InteractionArea.X, 0, iVHalfHeight));

	float halfHeight = Height / 2;

	ClimbVolume->SetBoxExtent(FVector(ClimbArea, halfHeight));
	ClimbVolume->SetRelativeLocation(FVector(ClimbArea.X, 0, halfHeight));

	EasyInteractBox->SetBoxExtent(FVector(EasyInteractArea, halfHeight));
	EasyInteractBox->SetRelativeLocation(FVector(0, 0, halfHeight));
}

void ALadder::Interact(APawn* InteractingPawn)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(InteractingPawn);

	if (!character)
		return;

	bInteractable = false;

	character->GetCharacterMovement<UDC_CMC>()->StartClimbing(this);

	LocalPlayerOnLadder = character;
	LocalPlayerOnLadder->GetCharacterMovement<UDC_CMC>()->OnStoppedClimbing.AddDynamic(this, &ALadder::StoppedInteracting);
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

void ALadder::StoppedInteracting()
{
	bInteractable = true;

	if(!IsValid(LocalPlayerOnLadder))
		return;

	LocalPlayerOnLadder->GetCharacterMovement<UDC_CMC>()->OnStoppedClimbing.RemoveAll(this);
	LocalPlayerOnLadder = nullptr;

}

void ALadder::PlaceLadder(const FVector& From, const FVector& To, const FVector& ForwardVector)
{
	SetActorLocation(From);

	FVector ladderVector = To - From;
	Height = ladderVector.Length();

	FRotator ladderRotation = UKismetMathLibrary::MakeRotFromZY(ladderVector, ForwardVector);
	SetActorRotation(ladderRotation);

	CalculateLadderBoxExtents();

}

