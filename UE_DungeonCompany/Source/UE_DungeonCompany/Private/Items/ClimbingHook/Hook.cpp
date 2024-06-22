// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook/Hook.h"
#include "Items/ItemData.h"
#include "Inventory/InventorySlot.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

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

	if (bAttached)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Attached"));
		HookMesh->SetSimulatePhysics(false);
		HookMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		HookMesh->OnComponentHit.AddDynamic(this, &AHook::OnHookHit);
	}

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
		bAttached = true;
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
}

void AHook::OnHookHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsValid(OtherActor) || !OtherActor->IsRootComponentStatic())
		return;

	HookMesh->SetSimulatePhysics(false);

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

	DOREPLIFETIME(AHook, bAttached)
}
