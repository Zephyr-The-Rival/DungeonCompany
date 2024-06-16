// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook.h"
#include "Items/ClimbingHook_Data.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "Inventory/InventorySlot.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "CableComponent.h"

AClimbingHook::AClimbingHook()
{
	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookMesh"));
	RootComponent = HookMesh;

	HookMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	HookMesh->SetSimulatePhysics(true);

	EasyInteract = CreateDefaultSubobject<USphereComponent>(TEXT("EasyInteract"));
	EasyInteract->SetupAttachment(RootComponent);

	EasyInteract->SetCollisionProfileName("EasyInteract");

}

void AClimbingHook::BeginPlay()
{
	Super::BeginPlay();

	AClimbingHook* partnerHook = Cast<UClimbingHook_Data>(MyData)->GetPartnerHook();

	if(!IsValid(partnerHook))
		return;

	AttachRopeToHooks(this, partnerHook);
}

void AClimbingHook::Interact(APawn* InteractingPawn)
{
	if (InteractingPawn == CharacterHoldingPartnerHook)
		return;
	
	Super::Interact(InteractingPawn);
}

void AClimbingHook::TriggerLocalPrimaryAction_Implementation(APlayerCharacter* User)
{

}

void AClimbingHook::TriggerPrimaryAction_Implementation(APlayerCharacter* User)
{
	
}

void AClimbingHook::TriggerSecondaryAction_Implementation(APlayerCharacter* User)
{
	FVector attachLocation = GetAttachLocation(User);
	attachLocation = GetActorLocation();

	if (attachLocation == FVector::ZeroVector)
		return;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(attachLocation);

	AClimbingHook* newClimbingHook = GetWorld()->SpawnActorDeferred<AClimbingHook>(BP_StaticClass, SpawnTransform);

	if(!newClimbingHook)
		return;

	newClimbingHook->HookMesh->SetSimulatePhysics(false);
	newClimbingHook->HookMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	newClimbingHook->SerializedStringData = SerializedStringData;
	newClimbingHook->FinishSpawning(SpawnTransform);

	UClimbingHook_Data* myData = Cast<UClimbingHook_Data>(MyData);

	AClimbingHook* partnerHook = myData->GetPartnerHook();

	if (!partnerHook)
	{
		myData->SetPartnerHook(newClimbingHook);
		return;
	}

	AttachRopeToHooks(newClimbingHook, partnerHook);

}

void AClimbingHook::TriggerLocalSecondaryAction_Implementation(APlayerCharacter* User)
{
	User->GetCurrentlyHeldInventorySlot()->MyItem = nullptr;
}

void AClimbingHook::OnHoldingInHand_Implementation(bool LocallyControlled)
{
	HookMesh->SetSimulatePhysics(false);
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

void AClimbingHook::AttachRopeToHooks(AClimbingHook* From, AClimbingHook* To)
{
	Rope = NewObject<UCableComponent>(From);
	//Rope->AttachToComponent(From->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	Rope->CableLength = MaxAttachDistance;
	Rope->CableWidth = 4.f;
	Rope->RegisterComponent();

	Rope->SetAttachEndToComponent(To->GetRootComponent());
}
