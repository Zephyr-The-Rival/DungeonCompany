// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook/Hook.h"
#include "Items/ItemData.h"
#include "Inventory/InventorySlot.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

void AHook::UpdateState()
{
	UpdateHookBehavior();
}

void AHook::UpdateHookBehavior()
{
	switch (State)
	{
	case HookState::InHand:
		HookMesh->SetSimulatePhysics(false);
		HookMesh->SetCollisionProfileName(FName("NoCollision"));
		HookMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case HookState::InWorldInactive:
		HookMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		HookMesh->SetCollisionProfileName(FName("BlockAll"));
		HookMesh->SetSimulatePhysics(true);
		break;

	case HookState::InWorldActive:
		HookMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		HookMesh->SetCollisionProfileName(FName("BlockAll"));
		HookMesh->SetSimulatePhysics(true);
		break;

	case HookState::InWorldAttached:
		HookMesh->SetSimulatePhysics(false);
		HookMesh->SetCollisionProfileName(FName("BlockAll"));
		HookMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid Hook State"));
	}

	if(!HasAuthority())
		return;

	if (State != HookState::InWorldActive)
	{
		HookMesh->OnComponentHit.RemoveAll(this);
	}
	else if (!HookMesh->OnComponentHit.IsBound())
	{
		HookMesh->OnComponentHit.AddDynamic(this, &AHook::OnHookHit);
	}
}

AHook::AHook()
{
	bReplicates = true;
	SetReplicateMovement(true);

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookMesh"));
	RootComponent = HookMesh;

	HookMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	HookMesh->SetSimulatePhysics(true);

	EasyInteract = CreateDefaultSubobject<USphereComponent>(TEXT("EasyInteract"));
	EasyInteract->SetupAttachment(RootComponent);

	EasyInteract->SetCollisionProfileName("EasyInteract");

}

void AHook::BeginPlay()
{
	Super::BeginPlay();

	UpdateState();

}

void AHook::TriggerPrimaryAction_Implementation(APlayerCharacter* User)
{
	
	FTransform SpawnTransform;

	FVector direction = User->GetLookDirection();

	SpawnTransform.SetLocation(User->FirstPersonCamera->GetComponentLocation() + direction * 100.f);

	AHook* newClimbingHook = GetWorld()->SpawnActorDeferred<AHook>(MyData->MyWorldItemClass, SpawnTransform);

	if (newClimbingHook)
	{
		newClimbingHook->State = HookState::InWorldActive;
		newClimbingHook->SerializedStringData = SerializedStringData;
		newClimbingHook->FinishSpawning(SpawnTransform);
	}

	User->ClearCurrentlyHeldInventorySlot();	
	newClimbingHook->Multicast_ThrowHook(direction);

	Destroy(true, true);
}

void AHook::TriggerSecondaryAction_Implementation(APlayerCharacter* User)
{
	FHitResult attachHit = GetAttachHit(User);

	if (!attachHit.bBlockingHit)
		return;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(attachHit.Location);

	AHook* newClimbingHook = GetWorld()->SpawnActorDeferred<AHook>(MyData->MyWorldItemClass, SpawnTransform);

	if (newClimbingHook)
	{
		newClimbingHook->State = HookState::InWorldAttached;
		newClimbingHook->SerializedStringData = SerializedStringData;
		newClimbingHook->FinishSpawning(SpawnTransform);
	}

	User->ClearCurrentlyHeldInventorySlot();
	Destroy(true, true);

}

void AHook::OnHoldingInHand_Implementation(bool LocallyControlled)
{
	HookMesh->SetSimulatePhysics(false);
	HookMesh->SetCollisionProfileName("NoCollision");
	EasyInteract->SetCollisionProfileName("NoCollision");

	State = HookState::InHand;
	UpdateState();
}

void AHook::Multicast_ThrowHook_Implementation(FVector Direction)
{
	if (State != HookState::InWorldActive)
	{
		State = HookState::InWorldActive;
		UpdateState();
	}

	GetHookMesh()->AddImpulse(Direction * 1250.f * HookMesh->GetMass());
}

void AHook::OnHookHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (State != HookState::InWorldActive || !IsValid(OtherActor) || OtherActor->GetRootComponent()->Mobility != EComponentMobility::Static)
		return;

	State = HookState::InWorldAttached;
	UpdateState();

}

FHitResult AHook::GetAttachHit(APlayerCharacter* User)
{
	FHitResult hit;

	if (!IsValid(User) || !IsValid(User->FirstPersonCamera))
		return hit;

	FVector start = User->FirstPersonCamera->GetComponentLocation();
	FVector direction = User->GetLookDirection();
	FVector end = direction * MaxAttachDistance + start;

	GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_GameTraceChannel3);

	return hit;
}

void AHook::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHook, State);
}
