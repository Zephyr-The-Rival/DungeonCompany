// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook/Rope.h"

#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsConstraintActor.h"
#include "Net/UnrealNetwork.h"

void ARope::SetAttachingActor(AActor* InActor)
{
	AttachingActor = InActor;
}

ARope::ARope()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RopeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RopeMesh"));
	RootComponent = RopeMesh;

	RopeMesh->SetSimulatePhysics(true);
	RopeMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

}

void ARope::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(AttachingActor))
		return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("AttachedingActor exists"));

	AttachToActor(AttachingActor, FAttachmentTransformRules::KeepRelativeTransform);	

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(GetActorLocation());

	PhysicsConstraintActor = GetWorld()->SpawnActorDeferred<APhysicsConstraintActor>(APhysicsConstraintActor::StaticClass(), SpawnTransform);

	PhysicsConstraintActor->GetConstraintComp()->ConstraintActor1 = this;
	PhysicsConstraintActor->GetConstraintComp()->ConstraintActor2 = AttachingActor;

	PhysicsConstraintActor->GetConstraintComp()->SetDisableCollision(true);

	PhysicsConstraintActor->FinishSpawning(SpawnTransform);

}

void ARope::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(PhysicsConstraintActor))
		PhysicsConstraintActor->Destroy();

	Super::EndPlay(EndPlayReason);
}

void ARope::Interact(APawn* InteractingPawn)
{
}

void ARope::AuthorityInteract(APawn* InteractingPawn)
{
}

void ARope::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARope, AttachingActor);
}


