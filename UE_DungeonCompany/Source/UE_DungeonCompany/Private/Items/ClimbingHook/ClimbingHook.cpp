// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook/ClimbingHook.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "Inventory/InventorySlot.h"
#include "Items/ClimbingHook/RopeEndComponent.h"
#include "WorldActors/Ladder.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "CableComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/ConstraintInstance.h"

AClimbingHook::AClimbingHook()
{
	bReplicates = true;

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookMesh"));
	RootComponent = HookMesh;

	HookMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	HookMesh->SetSimulatePhysics(true);

	EasyInteract = CreateDefaultSubobject<USphereComponent>(TEXT("EasyInteract"));
	EasyInteract->SetupAttachment(RootComponent);

	EasyInteract->SetCollisionProfileName("EasyInteract");

	//Rope Setup

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));
	PhysicsConstraint->SetupAttachment(RootComponent);

	PhysicsConstraint->SetLinearXLimit(LCM_Limited, RopeLength);
	PhysicsConstraint->SetLinearYLimit(LCM_Limited, RopeLength);
	PhysicsConstraint->SetLinearZLimit(LCM_Limited, RopeLength);
	PhysicsConstraint->SetAngularTwistLimit(ACM_Locked, 0);

	RopeEnd = CreateDefaultSubobject<URopeEndComponent>(TEXT("RopeEnd"));
	RopeEnd->SetupAttachment(RootComponent);

	RopeEnd->SetStaticMesh(RopeEndMesh);
	RopeEnd->SetIsReplicated(true);

	Rope = CreateDefaultSubobject<UCableComponent>(TEXT("Rope"));
	Rope->SetupAttachment(RootComponent);

	Rope->bAttachEnd = true;
	Rope->EndLocation = FVector::ZeroVector;
	Rope->CableLength = RopeLength;
	Rope->CableWidth = RopeWidth;
	Rope->NumSegments = RopeSegmentsNum;
	Rope->bEnableCollision = true;

	PhysicsConstraint->SetConstrainedComponents(HookMesh, NAME_None, RopeEnd, NAME_None);

}

void AClimbingHook::BeginPlay()
{
	Super::BeginPlay();

	Rope->SetAttachEndToComponent(RopeEnd);

	if(!HasAuthority())
		return;

	if (State != ClimbingHookState::InWorldActive)
	{
		Rope->DestroyComponent();
		RopeEnd->DestroyComponent();
	}
	else
	{
		FTimerHandle timerHandle;

		HookMesh->OnComponentHit.AddDynamic(this, &AClimbingHook::OnHookHit);
		GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &AClimbingHook::CreateLadders, 10.f);
	}
		
}

void AClimbingHook::TriggerLocalPrimaryAction_Implementation(APlayerCharacter* User)
{

}

void AClimbingHook::TriggerPrimaryAction_Implementation(APlayerCharacter* User)
{
	
}

void AClimbingHook::TriggerSecondaryAction_Implementation(APlayerCharacter* User)
{

	FHitResult attachHit = GetAttachHit(User);

	if(!attachHit.bBlockingHit)
		return;

	AttachNormal = attachHit.Normal;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(attachHit.Location);

	AClimbingHook* newClimbingHook = GetWorld()->SpawnActorDeferred<AClimbingHook>(BP_StaticClass, SpawnTransform);

	if (newClimbingHook)
	{
		newClimbingHook->HookMesh->SetSimulatePhysics(false);
		newClimbingHook->HookMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		newClimbingHook->SerializedStringData = SerializedStringData;
		newClimbingHook->State = ClimbingHookState::InWorldActive;
		newClimbingHook->FinishSpawning(SpawnTransform);
	}

	Destroy(true, true);

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

	State = ClimbingHookState::InHand;
}

void AClimbingHook::OnHookHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(!IsValid(OtherActor) || !OtherActor->IsRootComponentStatic())
		return;

	AttachNormal = Hit.Normal;
	HookMesh->SetSimulatePhysics(false);

}

FHitResult AClimbingHook::GetAttachHit(APlayerCharacter* User)
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

void AClimbingHook::CreateLadders()
{
	TArray<FVector> edgeLocations;

	GetEdgeLocations(edgeLocations);

	int locationsNum = edgeLocations.Num();

	for (int i = 0; i < locationsNum - 1; ++i)
	{

		if ((edgeLocations[i].Z - edgeLocations[i + 1].Z) < MinLadderHeight)
			continue;

		UE_LOG(LogTemp, Warning, TEXT("%s"), *edgeLocations[i].ToString());

		FVector ladderForwardVector;

		if (AttachNormal.Cross(FVector::UpVector).Length() < 0.5)
		{
			ladderForwardVector = edgeLocations[i + 1] - GetActorLocation();
			ladderForwardVector.Z = 0.f;
		}
		else
		{
			ladderForwardVector = AttachNormal;
		}

		ALadder* newLadder = GetWorld()->SpawnActor<ALadder>(LadderClass);
		//newLadder->SetIgnoreInteractionVolume(true);

		newLadder->PlaceLadder(edgeLocations[i+1], edgeLocations[i], ladderForwardVector);
	}
}

void AClimbingHook::GetEdgeLocations(TArray<FVector>& Out)
{
	TArray<FVector> ropeLocations;
	Rope->GetCableParticleLocations(ropeLocations);

	float halfSegmentLengths = 0.5f * (Rope->CableLength / Rope->NumSegments);

	int locationsNum = ropeLocations.Num();

	bool bOnFloor = false;

	Out.Add(HookMesh->GetComponentLocation());

	for (int i = 0; i < locationsNum - 1; ++i)
	{
		if (!bOnFloor && ropeLocations[i].Z - ropeLocations[i + 1].Z < halfSegmentLengths)
		{
			bOnFloor = true;
			Out.Add(ropeLocations[i+1]);
		}
		else if (bOnFloor && ropeLocations[i].Z - ropeLocations[i + 1].Z > halfSegmentLengths)
		{
			bOnFloor = false;
			Out.Add(ropeLocations[i + 1]);
		}
	}

}