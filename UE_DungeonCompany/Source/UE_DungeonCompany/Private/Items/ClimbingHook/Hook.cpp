// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook/Hook.h"
#include "Items/ItemData.h"
#include "Inventory/InventorySlot.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

void AHook::UpdateState()
{

	if (State != HookState::InWorldActive)
	{
		HookMesh->OnComponentHit.RemoveAll(this);
	}
	else if(!HookMesh->OnComponentHit.IsBound())
	{
		HookMesh->OnComponentHit.AddDynamic(this, &AHook::OnHookHit);
	}

	switch (State)
	{
		case HookState::InHand:
			HookMesh->SetSimulatePhysics(false);
			HookMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;

		case HookState::InWorldInactive:
			HookMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			HookMesh->SetSimulatePhysics(true);
			break;
			
		case HookState::InWorldActive:
			HookMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			HookMesh->SetSimulatePhysics(true);
			break;

		case HookState::InWorldAttached:
			HookMesh->SetSimulatePhysics(false);
			HookMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("Invalid Hook State"));
	}
}

AHook::AHook()
{
	bReplicates = true;

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

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *GetOwner()->GetName());

	UpdateState();

}

void AHook::TriggerPrimaryAction_Implementation(APlayerCharacter* User)
{

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

void AHook::OnHookHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsValid(OtherActor) || !OtherActor->IsRootComponentStatic())
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
	FVector direction = User->FirstPersonCamera->GetForwardVector();
	FVector end = direction * MaxAttachDistance + start;

	GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_GameTraceChannel3);

	return hit;
}

void AHook::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHook, State);
}
