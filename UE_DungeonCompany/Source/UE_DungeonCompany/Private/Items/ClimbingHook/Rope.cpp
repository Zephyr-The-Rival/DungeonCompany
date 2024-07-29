// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook/Rope.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "PlayerCharacter/Components/DC_CMC.h"

#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsConstraintActor.h"
#include "Components/PoseableMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"

ARope::ARope()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
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

	bStartedMoving = !IsRopeSettled(DeltaSeconds, true);
}

bool ARope::IsRopeSettled(float DeltaTime, bool bIgnoreIfMovedBefore)
{
	if(!bIgnoreIfMovedBefore && !bStartedMoving)
		return false;

	int boneNum = BoneNames.Num();

	if (LastRopeLocations.Num() == 0)
	{
		GetBoneLocations(LastRopeLocations);
		return false;
	}

	bool isSettled = true;

	for (int i = 0; i < boneNum; i++)
	{
		int boneIndex = RopeMesh->GetBoneIndex(BoneNames[i]);
		FVector location = RopeMesh->GetBoneTransform(boneIndex).GetLocation();

		if(isSettled && (location - LastRopeLocations[i]).Length() > SettledMaxRopeDelta)
			isSettled = false;

		LastRopeLocations[i] = location;		
	}

	return isSettled;
}

void ARope::Interact(APawn* InteractingPawn)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(InteractingPawn);

	if (!character)
		return;

	bInteractable = false;

	character->GetCharacterMovement<UDC_CMC>()->StartClimbing(this);

	if (!character->GetCharacterMovement<UDC_CMC>()->OnStoppedClimbing.IsBound())
		character->GetCharacterMovement<UDC_CMC>()->OnStoppedClimbing.AddDynamic(this, &ARope::StoppedInteracting);

}

void ARope::AuthorityInteract(APawn* InteractingPawn)
{

}

void ARope::StoppedInteracting(APlayerCharacter* PlayerCharacter)
{
	if(!IsValid(PlayerCharacter))
		return;

	bInteractable = true;

	PlayerCharacter->GetCharacterMovement<UDC_CMC>()->OnStoppedClimbing.RemoveAll(this);
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

void ARope::GetBoneLocations(TArray<FVector>& OutLocations)
{
	int bonesCount = BoneNames.Num();

	for (int i = 0; i < bonesCount; ++i)
	{
		int boneIndex = RopeMesh->GetBoneIndex(BoneNames[i]);
		OutLocations.Add(RopeMesh->GetBoneTransform(boneIndex).GetLocation());
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

FVector ARope::GetWorldLocationAtDistance(float Distance)
{
	if(Distance > 1000.f)
		return GetLowerEndLocation();

	if(Distance < 0.f)
		return GetUpperEndLocation();

	return FVector::ZeroVector;
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
	FixedRopeMesh->SetSkeletalMesh(RopeMesh->GetSkeletalMeshAsset(), false);

	FixedRopeMesh->GetBoneNames(BoneNames);

	int bonesNum = BoneNames.Num();
	int ropeTranNum = RopeTransforms.Num();

	SplineComponent = NewObject<USplineComponent>(this);
	SplineComponent->RegisterComponent();
	SplineComponent->ClearSplinePoints();

	for (int i = 0; i < bonesNum && i < ropeTranNum; ++i)
	{
		UBoxComponent* newBox = NewObject<UBoxComponent>(this);

		FixedRopeMesh->SetBoneTransformByName(BoneNames[i], RopeTransforms[i], EBoneSpaces::WorldSpace);
		newBox->InitBoxExtent(FVector(10, 10, 10));
		newBox->SetRelativeLocation(FVector(0, 0, -100));
		newBox->SetCollisionProfileName(FName("EasyInteract"));

		newBox->RegisterComponent();
		newBox->AttachToComponent(FixedRopeMesh, FAttachmentTransformRules::KeepRelativeTransform);
		newBox->SetWorldLocation(RopeTransforms[i].GetLocation());

		UBoxComponent* climbVolume = NewObject<UBoxComponent>(this);
		climbVolume->InitBoxExtent(FVector(50, 50, 5));
		climbVolume->RegisterComponent();
		climbVolume->AttachToComponent(FixedRopeMesh, FAttachmentTransformRules::KeepRelativeTransform);
		climbVolume->SetWorldLocation(RopeTransforms[i].GetLocation());

		RegisterClimbVolume(climbVolume);

		SplineComponent->AddSplineWorldPoint(RopeTransforms[i].GetLocation());
	}

	SplineComponent->SetUpVectorAtSplinePoint(0, SplineComponent->GetUpVectorAtSplinePoint(1, ESplineCoordinateSpace::World), ESplineCoordinateSpace::World);

	RopeMesh->SetSimulatePhysics(false);
	RopeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RopeMesh->SetVisibility(false);

	FixedRopeMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	BoneLength = 1000.f / (bonesNum - 1);

}

FName ARope::GetBoneNameNearestToZ(double Z) const
{
	CachedBoneIndex =	Z > GetUpperEndLocation().Z ? 1 : 
						Z < GetLowerEndLocation().Z ? BoneNames.Num() - 1 : 
						CachedBoneIndex < 0 ? GetBoneIndexNearestToZBinary(Z) :
						GetBoneIndexNearestToZLinear(Z);

	return BoneNames[CachedBoneIndex];
}

int ARope::GetBoneIndexNearestToZLinear(double Z) const
{
	FVector cachedBoneLocation = FixedRopeMesh->GetBoneLocationByName(BoneNames[CachedBoneIndex], EBoneSpaces::WorldSpace);

	double lastDelta  = cachedBoneLocation.Z - Z;
	if(lastDelta < 0)
		lastDelta *= -1;

	int updation = cachedBoneLocation.Z > Z? 1 : -1;

	int boneNum = BoneNames.Num();

	int i = CachedBoneIndex + updation;

	for (; (updation == 1 && i < boneNum - 1) || (updation == -1 && i > 0); i += updation)
	{
		double currentZ = FixedRopeMesh->GetBoneLocationByName(BoneNames[i], EBoneSpaces::WorldSpace).Z;
		double currentDelta = currentZ - Z;

		if(currentDelta < 0)
			currentDelta *= -1;

		if(currentDelta > lastDelta)
			return i-updation;

		lastDelta = currentDelta;
	}

	return i;
}

int ARope::GetBoneIndexNearestToZBinary(double Z) const
{
	int bonesNamesNum = BoneNames.Num();

	int bottom = 0;
	int top = bonesNamesNum - 1;

	int mid = top / 2;

	int closestIndex = mid;
	double closestZ = FixedRopeMesh->GetBoneLocationByName(BoneNames[mid], EBoneSpaces::WorldSpace).Z;

	bool closestValueFound = false;

	while (!closestValueFound)
	{
		closestValueFound = mid == closestIndex + 1 || mid == closestIndex - 1 || top - bottom < 2;

		double currentZ = FixedRopeMesh->GetBoneLocationByName(BoneNames[mid], EBoneSpaces::WorldSpace).Z;

		double currentDelta = currentZ - Z;
		double closestDelta = closestZ - Z;

		if (currentDelta < 0.f)
			currentDelta *= -1;

		if (closestDelta < 0.f)
			closestDelta *= -1;

		if (currentDelta < closestDelta)
		{
			closestIndex = mid;
			closestZ = FixedRopeMesh->GetBoneLocationByName(BoneNames[mid], EBoneSpaces::WorldSpace).Z;
		}

		if (closestValueFound)
			continue;

		if (currentZ > Z)
			bottom = mid + 1;
		else
			top = mid - 1;

		mid = (bottom + top) / 2;
	}

	CachedBoneIndex = closestIndex;

	return closestIndex;
}

FVector ARope::GetBoneUpVectorByName(FName BoneName) const
{
	FRotator boneRotation = FixedRopeMesh->GetBoneRotationByName(BoneName, EBoneSpaces::WorldSpace);
	boneRotation.Pitch += 90.;

	FVector testLocation = FixedRopeMesh->GetBoneLocationByName(BoneName, EBoneSpaces::WorldSpace);

	return boneRotation.Vector();
}

FVector ARope::GetLocationAtZ(double Z) const
{
	FVector tempLocation = GetActorLocation();
	tempLocation.Z = Z;
	return SplineComponent->FindLocationClosestToWorldLocation(tempLocation, ESplineCoordinateSpace::World);
}

FVector ARope::GetUpVectorAtZ(double Z) const
{
	FVector tempLocation = GetActorLocation();
	tempLocation.Z = Z;

	return SplineComponent->FindDirectionClosestToWorldLocation(tempLocation, ESplineCoordinateSpace::World);
}

double ARope::GetClimbRotationYaw(AActor* ClimbingActor) const
{
	return ClimbingActor->GetActorRotation().Yaw;
}

void ARope::CalculateLowerEndLocation() const
{
	int boneNum = BoneNames.Num();
	FName lastBoneLocation = BoneNames[boneNum - 1];

	FVector boneLocation = FixedRopeMesh->GetBoneLocationByName(lastBoneLocation, EBoneSpaces::WorldSpace);
	FVector boneUpVector = GetBoneUpVectorByName(lastBoneLocation);

	LowerEnd = boneLocation - boneUpVector * 0.5f;
}