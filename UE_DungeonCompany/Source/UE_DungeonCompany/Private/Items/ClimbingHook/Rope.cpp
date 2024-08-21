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
#include "UI/PlayerHud/PlayerHud.h"

ARope::ARope()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bInteractable = false;

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

	PhysicsConstraintActor = GetWorld()->SpawnActorDeferred<APhysicsConstraintActor>(
		APhysicsConstraintActor::StaticClass(), SpawnTransform);

	PhysicsConstraintActor->GetConstraintComp()->ConstraintActor1 = this;
	PhysicsConstraintActor->GetConstraintComp()->ConstraintActor2 = actorAttachedTo;

	PhysicsConstraintActor->GetConstraintComp()->ConstraintInstance.EnableParentDominates();

	PhysicsConstraintActor->GetConstraintComp()->SetLinearXLimit(LCM_Limited, 10.f);
	PhysicsConstraintActor->GetConstraintComp()->SetLinearYLimit(LCM_Limited, 10.f);
	PhysicsConstraintActor->GetConstraintComp()->SetLinearZLimit(LCM_Limited, 10.f);

	PhysicsConstraintActor->GetConstraintComp()->SetDisableCollision(false);
	PhysicsConstraintActor->FinishSpawning(SpawnTransform);

	PhysicsConstraintActor->AttachToActor(actorAttachedTo, FAttachmentTransformRules::KeepRelativeTransform);

	RopeMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	RopeMesh->SetSimulatePhysics(true);
}

void ARope::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	if (bStartedMoving)
	{
		SetActorTickEnabled(false);
		return;
	}

	bStartedMoving = !IsRopeSettled(DeltaSeconds, true);
}

bool ARope::IsRopeSettled(float DeltaTime, bool bIgnoreIfMovedBefore)
{
	if (!bIgnoreIfMovedBefore && !bStartedMoving)
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

		if (isSettled && (location - LastRopeLocations[i]).Length() > SettledMaxRopeDelta)
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

void ARope::SetMassScale(float InMassScale) const
{
	FixedRopeMesh->SetAllMassScale(InMassScale);
}

void ARope::RegisterInteractionVolume(UShapeComponent* InInteractionVolume)
{
	InteractionVolumes.Add(InInteractionVolume);

	InInteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &ARope::OnInteractVolumeEntered);
	InInteractionVolume->OnComponentEndOverlap.AddDynamic(this, &ARope::OnInteractVolumeLeft);
}

void ARope::StoppedInteracting(APlayerCharacter* PlayerCharacter)
{
	if (!IsValid(PlayerCharacter))
		return;

	bInteractable = true;

	PlayerCharacter->GetCharacterMovement<UDC_CMC>()->OnStoppedClimbing.RemoveAll(this);
}

void ARope::OnInteractVolumeEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                    const FHitResult& SweepResult)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);
	if (!character || !character->IsLocallyControlled())
		return;

	if (!IVolumesOverlappingCharacter.Contains(character))
	{
		IVolumesOverlappingCharacter.Add(character);
		bInteractable = true;
	}

	IVolumesOverlappingCharacter[character].Add(OverlappedComponent);
}

void ARope::OnInteractVolumeLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);
	if (!character || !character->IsLocallyControlled() || !IVolumesOverlappingCharacter.Contains(character))
		return;

	IVolumesOverlappingCharacter[character].Remove(OverlappedComponent);

	if (IVolumesOverlappingCharacter[character].Num() < 1)
	{
		bInteractable = false;
		IVolumesOverlappingCharacter.Remove(character);
	}
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

FVector ARope::GetWorldLocationAtDistance(float Distance)
{
	if (Distance > 1000.f)
		return GetLowerEndLocation();

	if (Distance < 0.f)
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
	FixedRopeMesh->SetSkinnedAssetAndUpdate(RopeMesh->GetSkeletalMeshAsset(), false);

	FixedRopeMesh->GetBoneNames(BoneNames);

	int bonesNum = BoneNames.Num();
	int ropeTranNum = RopeTransforms.Num();

	SplineComponent = NewObject<USplineComponent>(this);
	SplineComponent->RegisterComponent();
	SplineComponent->ClearSplinePoints();

	for (int i = 0; i < bonesNum && i < ropeTranNum; ++i)
	{
		FixedRopeMesh->SetBoneTransformByName(BoneNames[i], RopeTransforms[i], EBoneSpaces::WorldSpace);

		SplineComponent->AddSplineWorldPoint(RopeTransforms[i].GetLocation());
	}
	FRotator rotationOverride = SplineComponent->GetRotationAtSplinePoint(3, ESplineCoordinateSpace::World);
	SplineComponent->SetRotationAtSplinePoint(0, rotationOverride, ESplineCoordinateSpace::World);
	SplineComponent->SetRotationAtSplinePoint(1, rotationOverride, ESplineCoordinateSpace::World);
	SplineComponent->SetRotationAtSplinePoint(2, rotationOverride, ESplineCoordinateSpace::World);


	for (int i = 0; i < bonesNum && i < ropeTranNum; ++i)
	{
		FVector direction = SplineComponent->GetDirectionAtSplinePoint(i, ESplineCoordinateSpace::World);
		FVector location = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);

		DrawDebugLine(GetWorld(), location, location + direction * 50.f, FColor::Red, false, 5.f);
		if (SplineComponent->GetDirectionAtSplinePoint(i, ESplineCoordinateSpace::World).Dot(-FVector::UpVector) < 0.5)
			continue;

		UBoxComponent* newBox = NewObject<UBoxComponent>(this);

		FixedRopeMesh->SetBoneTransformByName(BoneNames[i], RopeTransforms[i], EBoneSpaces::WorldSpace);
		newBox->InitBoxExtent(FVector(10, 10, 10));
		newBox->SetRelativeLocation(FVector(0, 0, -100));
		newBox->SetCollisionProfileName(FName("EasyInteract"));

		newBox->RegisterComponent();
		newBox->AttachToComponent(FixedRopeMesh, FAttachmentTransformRules::KeepRelativeTransform);
		newBox->SetWorldLocation(RopeTransforms[i].GetLocation());

		UBoxComponent* climbVolume = NewObject<UBoxComponent>(this);
		climbVolume->InitBoxExtent(FVector(5, 25, 25));
		climbVolume->RegisterComponent();
		climbVolume->AttachToComponent(FixedRopeMesh, FAttachmentTransformRules::KeepRelativeTransform);
		climbVolume->SetWorldLocation(RopeTransforms[i].GetLocation());
		climbVolume->SetWorldRotation(SplineComponent->GetRotationAtSplinePoint(i, ESplineCoordinateSpace::World));

		RegisterClimbVolume(climbVolume);
		RegisterInteractionVolume(climbVolume);
	}

	RopeMesh->SetSimulatePhysics(false);
	RopeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RopeMesh->SetVisibility(false);

	FixedRopeMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	BoneLength = 1000.f / (bonesNum - 1);
}

float ARope::GetDistanceAtLocation(FVector ClimbingActorLocation) const
{
	return SplineComponent->GetSplineLength() - SplineComponent->GetDistanceAlongSplineAtLocation(
		ClimbingActorLocation, ESplineCoordinateSpace::World);
}

FVector ARope::GetLocationAtDistance(float Distance) const
{
	return SplineComponent->GetLocationAtDistanceAlongSpline(SplineComponent->GetSplineLength() - Distance,
	                                                         ESplineCoordinateSpace::World);
}

FVector ARope::GetUpVectorAtDistance(float Distance) const
{
	return -SplineComponent->GetDirectionAtDistanceAlongSpline(SplineComponent->GetSplineLength() - Distance,
	                                                           ESplineCoordinateSpace::World);
}

double ARope::GetClimbRotationYaw(AActor* ClimbingActor) const
{
	return ClimbingActor->GetActorRotation().Yaw;
}

void ARope::CalculateLowerEndLocation() const
{
	LowerEnd = SplineComponent->GetLocationAtDistanceAlongSpline(SplineComponent->GetSplineLength(),
	                                                             ESplineCoordinateSpace::World);
}

void ARope::OnHovered(APlayerCharacter* PlayerCharacter)
{
	PlayerCharacter->GetMyHud()->ShowTextInteractPrompt("Climb");
}
