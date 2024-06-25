// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook/Rope.h"

#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsConstraintActor.h"
#include "Components/PoseableMeshComponent.h"
#include "Net/UnrealNetwork.h"

ARope::ARope()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RopeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RopeMesh"));
	RootComponent = RopeMesh;

	RopeMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	RopeMesh->SetSimulatePhysics(true);

	FixedRopeMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("FixedRopeMesh"));
	FixedRopeMesh->SetupAttachment(RootComponent);

}

void ARope::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(HasAuthority());

	GetWorld()->GetTimerManager().SetTimer(CheckOwnerHandle, this, &ARope::SetupActorAttachment, 0.05f, true, 0.f);

	FTimerHandle timerhandle;
	FTimerDelegate timerDelegate = FTimerDelegate::CreateLambda([this]() {

		FreezeAndReplicate();

		});

	//GetWorld()->GetTimerManager().SetTimer(timerhandle, timerDelegate, 5.f, false);

	RopeMesh->GetBoneNames(BoneNames);

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

void ARope::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bStartedMoving)
	{
		SetActorTickEnabled(true);
		return;
	}

	bStartedMoving = !IsRopeSettled(true);
}

bool ARope::IsRopeSettled(bool bIgnoreIfMovedBefore)
{
	if(!bIgnoreIfMovedBefore && !bStartedMoving)
		return false;

	int boneNum = BoneNames.Num();
	for (int i = 0; i < boneNum; i++)
	{
		if (RopeMesh->GetBoneLinearVelocity(BoneNames[i]).Length() > SettledMaxLinearVelocity)
			return false;
	}

	return true;
}

void ARope::Interact(APawn* InteractingPawn)
{
}

void ARope::AuthorityInteract(APawn* InteractingPawn)
{
}

void ARope::GetRopeTransforms(USkinnedMeshComponent* InRopeMesh, TArray<FTransform>& OutTransforms)
{
	TArray<FName> boneNames;

	InRopeMesh->GetBoneNames(boneNames);

	int bonesCount = boneNames.Num();

	for (int i = 0; i < bonesCount; ++i)
	{
		int boneIndex = InRopeMesh->GetBoneIndex(boneNames[i]);
		OutTransforms.Add(InRopeMesh->GetBoneTransform(boneIndex));
	}
}

void ARope::GetEdgeLocations(TArray<FVector>& OutLocations)
{
	TArray<FTransform> ropeTransforms;
	GetRopeTransforms(RopeMesh, ropeTransforms);

	float halfSegmentLengths = 0.4f * (1000.f / 100.f);
	
	int transformsNum = ropeTransforms.Num();

	if (transformsNum < 4)
		return;
	
	bool bOnFloor = !FMath::IsNearlyEqual(ropeTransforms[1].GetLocation().Z, ropeTransforms[2].GetLocation().Z, halfSegmentLengths * 0.8)? false : FMath::IsNearlyEqual(ropeTransforms[1].GetLocation().Z, ropeTransforms[3].GetLocation().Z, halfSegmentLengths * 0.8);
	
	OutLocations.Add(ropeTransforms[1].GetLocation());

	for (int i = 2; i < transformsNum - 1; ++i)
	{
		if (!bOnFloor && ropeTransforms[i].GetLocation().Z - ropeTransforms[i + 1].GetLocation().Z < halfSegmentLengths)
		{
			bOnFloor = true;
			OutLocations.Add(ropeTransforms[i + 1].GetLocation());
		}
		else if (bOnFloor && ropeTransforms[i].GetLocation().Z - ropeTransforms[i + 1].GetLocation().Z > halfSegmentLengths)
		{
			bOnFloor = false;
			OutLocations.Add(ropeTransforms[i + 1].GetLocation());
		}
	}

	OutLocations.Add(ropeTransforms[transformsNum-1].GetLocation());
}

void ARope::FreezeAndReplicate()
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Rope freeze can only be initiated by the server."));
		return;
	}

	TArray<FTransform> ropeTransforms;
	GetRopeTransforms(RopeMesh, ropeTransforms);

	Multicast_SetTransformsAndFreeze(ropeTransforms);
}

void ARope::Multicast_SetTransformsAndFreeze_Implementation(const TArray<FTransform>& RopeTransforms)
{
	TArray<FName> boneNames;

	FixedRopeMesh->SetSkeletalMesh(RopeMesh->GetSkeletalMeshAsset(), false);

	FixedRopeMesh->GetBoneNames(boneNames);

	int bonesNum = boneNames.Num();
	int ropeTranNum = RopeTransforms.Num();

	UE_LOG(LogTemp, Warning, TEXT("%d"), bonesNum);
	for (int i = 0; i < bonesNum && i < ropeTranNum; ++i)
	{
		FixedRopeMesh->SetBoneTransformByName(boneNames[i], RopeTransforms[i], EBoneSpaces::WorldSpace);
	}

	RopeMesh->SetSimulatePhysics(false);
	RopeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RopeMesh->SetVisibility(false);
	
}
