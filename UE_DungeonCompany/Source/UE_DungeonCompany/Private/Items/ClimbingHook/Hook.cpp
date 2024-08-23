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
	case EHookState::InHand:
		HookMesh->SetSimulatePhysics(false);
		HookMesh->SetCollisionProfileName(FName("NoCollision"));
		HookMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EHookState::InWorldInactive:
		HookMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		HookMesh->SetCollisionProfileName(FName("WorldItem"));
		HookMesh->SetSimulatePhysics(true);
		break;

	case EHookState::InWorldActive:
		HookMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		HookMesh->SetCollisionProfileName(FName("WorldItem"));
		HookMesh->SetSimulatePhysics(true);
		break;

	case EHookState::InWorldAttached:
		HookMesh->SetSimulatePhysics(false);
		HookMesh->SetCollisionProfileName(FName("BlockAll"));
		HookMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid Hook State"));
	}

	if(!HasAuthority())
		return;

	if (State != EHookState::InWorldActive)
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
	HookMesh->SetNotifyRigidBodyCollision(true);

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
	
	if(this->GetHookState()== EHookState::InHandAfterThrow)
		return;

	User->AttackBlend=1;
	User->bSwitchHandAllowed=false;
	
	
	this->State=EHookState::InHandAfterThrow;
}

void AHook::TriggerSecondaryAction_Implementation(APlayerCharacter* User)
{

	if(this->GetHookState()== EHookState::InHandAfterThrow)
		return;

	FHitResult attachHit = GetAttachHit(User);

	if (!attachHit.bBlockingHit)
		return;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(attachHit.Location);

	AHook* newClimbingHook = GetWorld()->SpawnActorDeferred<AHook>(MyData->MyWorldItemClass, SpawnTransform);

	if (newClimbingHook)
	{
		newClimbingHook->State = EHookState::InWorldAttached;
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

	State = EHookState::InHand;
	UpdateState();
}

void AHook::Multicast_ThrowHook_Implementation(FVector Direction)
{
	if (State != EHookState::InWorldActive)
	{
		State = EHookState::InWorldActive;
		UpdateState();
	}

	GetHookMesh()->AddImpulse(Direction * 1250.f * HookMesh->GetMass());
}

void AHook::OnHookHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (State != EHookState::InWorldActive || !IsValid(OtherActor) || OtherActor->GetRootComponent()->Mobility != EComponentMobility::Static)
		return;

	State = EHookState::InWorldAttached;
	UpdateState();

}

FHitResult AHook::GetAttachHit(APlayerCharacter* User)
{
	FHitResult hit;

	if (!IsValid(User) || !IsValid(User->FirstPersonCamera))
		return hit;

	FVector start = User->FirstPersonCamera->GetComponentLocation();
	FVector direction = User->GetBaseAimDirection();
	FVector end = direction * MaxAttachDistance + start;

	GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_GameTraceChannel3);

	return hit;
}

void AHook::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHook, State);
}

void AHook::HookLetGo(APlayerCharacter* User)
{

	
	FTransform SpawnTransform;

	FVector direction = User->GetBaseAimDirection();

	SpawnTransform.SetLocation(User->FirstPersonCamera->GetComponentLocation() + direction * 100.f);

	AHook* newClimbingHook = GetWorld()->SpawnActorDeferred<AHook>(MyData->MyWorldItemClass, SpawnTransform);

	if (newClimbingHook)
	{
		newClimbingHook->State = EHookState::InWorldActive;
		newClimbingHook->SerializedStringData = SerializedStringData;
		newClimbingHook->FinishSpawning(SpawnTransform);
	}
	
	newClimbingHook->Multicast_ThrowHook(direction);

	this->HookMesh->SetVisibility(false);


}

void AHook::OnHookThrown(APlayerCharacter* User)
{
	User->AttackBlend=0;
	User->bSwitchHandAllowed=true;
	
	User->ClearCurrentlyHeldInventorySlot();
	Destroy(true, true);
}
