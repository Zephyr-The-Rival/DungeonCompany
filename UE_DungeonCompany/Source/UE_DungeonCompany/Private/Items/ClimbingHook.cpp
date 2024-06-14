// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

AClimbingHook::AClimbingHook()
{
	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookMesh"));
	RootComponent = HookMesh;

	HookMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EasyInteract = CreateDefaultSubobject<USphereComponent>(TEXT("EasyInteract"));
	EasyInteract->SetupAttachment(RootComponent);

	EasyInteract->SetCollisionProfileName("EasyInteract");

}

void AClimbingHook::Interact(APawn* InteractingPawn)
{
	if(PartnerHook)
	if (InteractingPawn == CharacterHoldingPartnerHook)
	{	
		return;
	}
	
	Super::Interact(InteractingPawn);
}

void AClimbingHook::TriggerPrimaryAction_Implementation(APlayerCharacter* User)
{
	
}

void AClimbingHook::TriggerSecondaryAction_Implementation(APlayerCharacter* User)
{
	FVector attachLocation = GetAttachLocation(User);

	if (attachLocation == FVector::ZeroVector)
		return;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	HookMesh->SetSimulatePhysics(false);

	SetActorLocation(attachLocation);
}

void AClimbingHook::OnHoldingInHand_Implementation(bool LocallyControlled)
{
	HookMesh->SetCollisionProfileName("NoCollision");
	EasyInteract->SetCollisionProfileName("NoCollision");
}

FVector AClimbingHook::GetAttachLocation(APlayerCharacter* User)
{
	if (!IsValid(User) || !IsValid(User->FirstPersonCamera))
		return FVector::ZeroVector;

	FVector start = User->FirstPersonCamera->GetComponentLocation();
	FVector direction = User->FirstPersonCamera->GetForwardVector();
	FVector end = direction * MaxAttachDistance + start;

	FHitResult hit;

	GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_GameTraceChannel3);

	if (!hit.bBlockingHit)
		return FVector::ZeroVector;


	return hit.Location;
}