// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/Climbable.h"
#include "PlayerCharacter/Components/DC_CMC.h"

#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"

// Sets default values
AClimbable::AClimbable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AClimbable::BeginPlay()
{
	Super::BeginPlay();

	SetupClimbVolumes();
}

void AClimbable::RegisterClimbVolume(UShapeComponent* InClimbVolume)
{
	ClimbVolumes.Add(InClimbVolume);

	if (!bClimbVolumeSetupCompleted)
		return;

	InClimbVolume->OnComponentBeginOverlap.AddDynamic(this, &AClimbable::OnClimbVolumeBeginOverlap);
	InClimbVolume->OnComponentEndOverlap.AddDynamic(this, &AClimbable::OnClimbVolumeEndOverlap);
}

void AClimbable::SetupClimbVolumes()
{
	int climbVolumeNum = ClimbVolumes.Num();

	UE_LOG(LogTemp, Warning, TEXT("%d"), climbVolumeNum);

	for (int i = 0; i < climbVolumeNum; ++i)
	{
		ClimbVolumes[i]->OnComponentBeginOverlap.AddDynamic(this, &AClimbable::OnClimbVolumeBeginOverlap);
		ClimbVolumes[i]->OnComponentEndOverlap.AddDynamic(this, &AClimbable::OnClimbVolumeEndOverlap);
	}

	bClimbVolumeSetupCompleted = true;
}

void AClimbable::OnClimbVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* character = Cast<ACharacter>(OtherActor);

	if (!character || !Cast<UDC_CMC>(character->GetCharacterMovement()))
		return;
	
	if (!VolumesOverlappingCharacter.Contains(character))
	{
		VolumesOverlappingCharacter.Add(character);
		character->GetCharacterMovement<UDC_CMC>()->ChangeClimbAllowedState(true);
	}

	VolumesOverlappingCharacter[character].Add(OverlappedComponent);
}

void AClimbable::OnClimbVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* character = Cast<ACharacter>(OtherActor);

	if (!character || !Cast<UDC_CMC>(character->GetCharacterMovement()) || !VolumesOverlappingCharacter.Contains(character))
		return;

	VolumesOverlappingCharacter[character].Remove(OverlappedComponent);

	if (VolumesOverlappingCharacter[character].Num() < 1)
	{
		character->GetCharacterMovement<UDC_CMC>()->ChangeClimbAllowedState(false);
		VolumesOverlappingCharacter.Remove(character);
	}
}

float AClimbable::GetDistanceAtLocation(FVector ClimbingActorLocation) const
{
	return 0.0f;
}

FVector AClimbable::GetLocationAtDistance(float Distance) const
{
	return FVector::ZeroVector;
}

FVector AClimbable::GetUpVectorAtDistance(float Distance) const
{
	return GetActorUpVector();
}

double AClimbable::GetClimbRotationYaw(AActor* ClimbingActor) const
{
	return GetActorRotation().Yaw + 180.;
}

FVector AClimbable::GetLowerEndLocation() const
{
	if(LowerEnd == FVector::ZeroVector)
		CalculateLowerEndLocation();

	return LowerEnd;
}

FVector AClimbable::GetUpperEndLocation() const
{
	if (UpperEnd == FVector::ZeroVector)
		CalculateUpperEndLocation();

	return UpperEnd;
}

void AClimbable::CalculateLowerEndLocation() const
{
	LowerEnd = GetActorLocation();
}

void AClimbable::CalculateUpperEndLocation() const
{
	UpperEnd = GetActorLocation();
}
