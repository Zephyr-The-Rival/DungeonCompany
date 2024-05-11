// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/FunGuy.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

AFunGuy::AFunGuy()
{
	CloudSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CloudSphere"));
	RootComponent = CloudSphere;

	GetCapsuleComponent()->SetupAttachment(RootComponent);

	TempMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TempMesh"));
	TempMesh->SetupAttachment(GetCapsuleComponent());

	NetUpdateFrequency = 100.f;
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetActive(false);
}

void AFunGuy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (AgeSeconds > MaxSizeAgeSeconds)
		AgeSeconds = MaxSizeAgeSeconds;

	FVector newScale = FVector(1, 1, 1);
	newScale += newScale * AgeSeconds * AgeBonusScaleMultiplier;

	GetCapsuleComponent()->SetRelativeScale3D(newScale);

	float height = (AgeSeconds < LiftoffAge) ? 0.f :
		(AgeSeconds > LiftoffAge + LiftoffTime) ? LiftoffHeight :
		(AgeSeconds - LiftoffAge) / LiftoffTime * LiftoffHeight;

	GetCapsuleComponent()->SetRelativeLocation(FVector::UpVector * height);
}

void AFunGuy::BeginPlay()
{
	Super::BeginPlay();

}

void AFunGuy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AgeSeconds >= MaxSizeAgeSeconds)
		return;

	AgeSeconds += DeltaSeconds * AgingMultiplier;

	if (AgeSeconds > MaxSizeAgeSeconds)
		AgeSeconds = MaxSizeAgeSeconds;

	FVector newScale = FVector(1, 1, 1);
	newScale += newScale * AgeSeconds * AgeBonusScaleMultiplier;

	GetCapsuleComponent()->SetRelativeScale3D(newScale);

	float height = (AgeSeconds < LiftoffAge) ? 0.f :
		(AgeSeconds > LiftoffAge + LiftoffTime) ? LiftoffHeight :
		(AgeSeconds - LiftoffAge) / LiftoffTime * LiftoffHeight;

	GetCapsuleComponent()->SetRelativeLocation(FVector::UpVector * height);

}

void AFunGuy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFunGuy, AgeSeconds);
}
