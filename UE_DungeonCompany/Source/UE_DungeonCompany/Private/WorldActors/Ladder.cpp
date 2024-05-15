// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/Ladder.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "PlayerCharacter/Components/DC_CMC.h"

// Sets default values
ALadder::ALadder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bInteractable = false;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	LadderMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("LadderMesh"));
	LadderMesh->SetupAttachment(RootComponent);

	BottomBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BottomBox"));
	BottomBox->SetupAttachment(RootComponent);

	BottomBox->InitBoxExtent(FVector(100, 100, 5));
	BottomBox->SetRelativeLocation(BottomBox->GetUnscaledBoxExtent().Z * FVector::UpVector);

	InteractVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractVolume->SetupAttachment(RootComponent);

	InteractVolume->InitBoxExtent(FVector(1, 1, 1));
}

void ALadder::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	LadderMesh->ClearInstances();

	if(!LadderSectionReference)
		return;

	LadderMesh->SetStaticMesh(LadderSectionReference);

	if(Material)
		LadderMesh->SetMaterial(0, Material);
	
	for (unsigned int i = 0; i < SectionsCount; ++i)
	{
		FVector translation = FVector::UpVector * (i * SectionHeight);
		LadderMesh->AddInstance(FTransform(translation), false);	
	}

	if(bSectionOriginInMid)
		LadderMesh->SetRelativeLocation(FVector(-SectionDepth, 0, SectionHeight/2));

	float LadderHalfHeight = (SectionHeight / 2) * SectionsCount;

	InteractVolume->SetBoxExtent(FVector(InteractionArea, LadderHalfHeight));
	InteractVolume->SetRelativeLocation(FVector(InteractionArea.X, 0, LadderHalfHeight));

}

// Called when the game starts or when spawned
void ALadder::BeginPlay()
{
	Super::BeginPlay();

	BottomBox->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnBottomBoxOverlap);

	InteractVolume->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnInteractVolumeEntered);
	InteractVolume->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnInteractVolumeLeft);
	
}

void ALadder::Interact(APawn* InteractingPawn)
{

	APlayerCharacter* character = Cast<APlayerCharacter>(InteractingPawn);
	
	if(!character)
		return;

	float distanceToLadder = character->GetCapsuleComponent()->GetScaledCapsuleRadius();

	float distanceToStart = character->GetDistanceTo(this);
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

