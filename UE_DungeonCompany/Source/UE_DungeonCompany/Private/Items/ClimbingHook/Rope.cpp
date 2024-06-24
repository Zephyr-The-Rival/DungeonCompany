// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook/Rope.h"

#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsConstraintActor.h"
#include "Net/UnrealNetwork.h"

ARope::ARope()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RopeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RopeMesh"));
	RootComponent = RopeMesh;

	RopeMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	RopeMesh->SetSimulatePhysics(true);

}

void ARope::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(CheckOwnerHandle, this, &ARope::SetupActorAttachment, 0.05f, true, 0.f);

	return;

	FTimerHandle timerhandle;
	FTimerDelegate timerDelegate = FTimerDelegate::CreateLambda([this]() {

		TArray<FVector> out;
		GetEdgeLocations(out);

	});

	GetWorld()->GetTimerManager().SetTimer(timerhandle, timerDelegate, 5.f, false);

}

void ARope::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(PhysicsConstraintActor))
		PhysicsConstraintActor->Destroy();

	Super::EndPlay(EndPlayReason);
}

void ARope::SetupActorAttachment()
{
	AActor* actorAttachedTo = GetOwner();

	if (!IsValid(actorAttachedTo))
		return;

	GetWorld()->GetTimerManager().ClearTimer(CheckOwnerHandle);

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(actorAttachedTo->GetActorLocation());

	PhysicsConstraintActor = GetWorld()->SpawnActorDeferred<APhysicsConstraintActor>(APhysicsConstraintActor::StaticClass(), SpawnTransform);

	PhysicsConstraintActor->GetConstraintComp()->ConstraintActor1 = this;
	PhysicsConstraintActor->GetConstraintComp()->ConstraintActor2 = actorAttachedTo;

	//PhysicsConstraintActor->GetConstraintComp()->SetLinearXLimit(LCM_Limited, 30.f);
	//PhysicsConstraintActor->GetConstraintComp()->SetLinearYLimit(LCM_Limited, 30.f);
	//PhysicsConstraintActor->GetConstraintComp()->SetLinearZLimit(LCM_Limited, 30.f);

	PhysicsConstraintActor->GetConstraintComp()->SetDisableCollision(true);

	PhysicsConstraintActor->FinishSpawning(SpawnTransform);

	RopeMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	RopeMesh->SetSimulatePhysics(true);
}

void ARope::Interact(APawn* InteractingPawn)
{
}

void ARope::AuthorityInteract(APawn* InteractingPawn)
{
}

void ARope::GetEdgeLocations(TArray<FVector>& Out)
{
	TArray<FVector> ropeLocations;

	TArray<FName> boneNames;

	RopeMesh = GetComponentByClass<USkeletalMeshComponent>();

	if(!RopeMesh)
		return;

	RopeMesh->SetSimulatePhysics(false);
	RopeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RopeMesh->GetBoneNames(boneNames);
	
	int bonesCount = boneNames.Num();
	
	for (int i = 1; i < bonesCount; ++i)
	{
		ropeLocations.Add(RopeMesh->GetBoneLocation(boneNames[i], EBoneSpaces::WorldSpace));
	}

	float halfSegmentLengths = 0.5f * (1000.f / 100.f);
	
	int locationsNum = ropeLocations.Num();

	if (locationsNum < 3)
		return;
	
	bool bOnFloor = !FMath::IsNearlyEqual(ropeLocations[0].Z, ropeLocations[1].Z, halfSegmentLengths * 0.8)? false : FMath::IsNearlyEqual(ropeLocations[0].Z, ropeLocations[2].Z, halfSegmentLengths * 0.8);
	
	for (int i = 0; i < locationsNum - 1; ++i)
	{
		if (!i)
		{
			Out.Add(ropeLocations[i]);
			continue;
		}

		if (!bOnFloor && ropeLocations[i].Z - ropeLocations[i + 1].Z < halfSegmentLengths)
		{
			bOnFloor = true;
			Out.Add(ropeLocations[i + 1]);
		}
		else if (bOnFloor && ropeLocations[i].Z - ropeLocations[i + 1].Z > halfSegmentLengths)
		{
			bOnFloor = false;
			Out.Add(ropeLocations[i + 1]);
		}
	}
}