// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/Ladder.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "PlayerCharacter/Components/DC_CMC.h"
#include "UI/PlayerHud/PlayerHud.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "UI/PlayerHud/PlayerHud.h"
#include "Kismet/KismetMathLibrary.h"

void ALadder::CalculateUpperEndLocation() const
{
	UpperEnd = GetActorLocation() + Height * GetActorUpVector();
}

float ALadder::GetDistanceAtLocation(FVector ClimbingActorLocation) const
{
	FVector location = GetActorLocation();

	float heightDelta = ClimbingActorLocation.Z - location.Z;

	if(heightDelta < 0.f)
		return 0.f;
	else if(heightDelta > Height)
		return Height;

	FVector ladderVector = Height * GetActorUpVector();

	ladderVector *= (heightDelta / ladderVector.Z);

	return ladderVector.Length();
}

FVector ALadder::GetLocationAtDistance(float Distance) const
{
	FVector location = GetActorLocation();

	if(Distance < 0.f)
		return GetLowerEndLocation();
	else if(Distance > Height)
		return GetUpperEndLocation();

	return location + GetActorUpVector() * Distance;
}

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

	EasyInteractBox->SetCollisionProfileName(FName("EasyInteract"));

	EasyInteractBox->InitBoxExtent(FVector(1, 1, 1));

	SetActorTickEnabled(false);

}

void ALadder::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	CalculateLadderBoxExtents();

}

// Called when the game starts or when spawned
void ALadder::BeginPlay()
{
	RegisterClimbVolume(ClimbVolume);

	Super::BeginPlay();

	if (!bIgnoreInteractionVolume)
	{
		InteractVolume->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnInteractVolumeEntered);
		InteractVolume->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnInteractVolumeLeft);
	}

	FTimerHandle timerHandle;
	FTimerDelegate timerDelegate = FTimerDelegate::CreateUObject(this, &ALadder::SetActorTickEnabled, true);

	GetWorld()->GetTimerManager().SetTimer(timerHandle, timerDelegate, 1.f, false);
	
}

void ALadder::CalculateLadderBoxExtents()
{
	float iVHalfHeight = Height / 2 + InteractionVolumeHeightBonus / 2;
	
	if(InteractionArea.X > ClimbArea.X)
		InteractionArea.X = ClimbArea.X;

	if (InteractionArea.Y > ClimbArea.Y)
		InteractionArea.Y = ClimbArea.Y;

	InteractVolume->SetBoxExtent(FVector(InteractionArea, iVHalfHeight));
	InteractVolume->SetRelativeLocation(FVector(InteractionArea.X, 0, iVHalfHeight));
	
	ClimbVolume->SetBoxExtent(FVector(ClimbArea, iVHalfHeight));
	ClimbVolume->SetRelativeLocation(FVector(ClimbArea.X, 0, iVHalfHeight));

	float halfHeight = Height / 2;

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

	if(!LocalPlayerOnLadder->GetCharacterMovement<UDC_CMC>()->OnStoppedClimbing.IsBound())
		LocalPlayerOnLadder->GetCharacterMovement<UDC_CMC>()->OnStoppedClimbing.AddDynamic(this, &ALadder::StoppedInteracting);
}

void ALadder::OnInteractVolumeEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);
	if(!character || !character->IsLocallyControlled())
		return;

	bInInteractionVolume = true;

	bInteractable = true;

}

void ALadder::OnInteractVolumeLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);
	if (!character || !character->IsLocallyControlled())
		return;

	bInInteractionVolume = false;

	bInteractable = false;

}

void ALadder::StoppedInteracting(APlayerCharacter* PlayerCharacter)
{
	if(bInInteractionVolume)
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

	FRotator ladderRotation = UKismetMathLibrary::MakeRotFromZX(ladderVector, ForwardVector);
	SetActorRotation(ladderRotation);

	CalculateLadderBoxExtents();

}

void ALadder::OnHovered(APlayerCharacter* PlayerCharacter)
{
	PlayerCharacter->GetMyHud()->ShowTextInteractPrompt("Climb");
}