// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook/ClimbingHook.h"
#include "Items/ClimbingHook/Rope.h"

AClimbingHook::AClimbingHook()
{
	PrimaryActorTick.TickInterval = 0.1f;
	bReplicates = true;
}

void AClimbingHook::BeginPlay()
{
	SetActorTickEnabled(false);
	Super::BeginPlay();

}

void AClimbingHook::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!HasAuthority())
		return;

	if(IsValid(SpawnedRope))
		SpawnedRope->Destroy();

	Super::EndPlay(EndPlayReason);
}

void AClimbingHook::UpdateHookBehavior()
{
	Super::UpdateHookBehavior();

	if (!HasAuthority())
		return;

	if(GetHookState() == HookState::InWorldAttached)
		SetActorTickEnabled(true);

	if ((GetHookState() == HookState::InWorldAttached || GetHookState() == HookState::InWorldActive) && !IsValid(SpawnedRope))
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;

		SpawnedRope = GetWorld()->SpawnActor<ARope>(RopeClass, GetActorLocation(), GetActorRotation(), spawnParams);
	}
	else if (GetHookState() != HookState::InWorldAttached && GetHookState() != HookState::InWorldActive && IsValid(SpawnedRope))
	{
		SpawnedRope->Destroy();
	}

}

void AClimbingHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!IsValid(SpawnedRope))
		return;

	RopeLifetime += DeltaTime;

	if (RopeLifetime > 0.5f && SpawnedRope->IsRopeSettled(DeltaTime) || RopeLifetime > RopeAutoFreezeTime)
	{
		SpawnedRope->FreezeAndReplicate();
		SetActorTickEnabled(false);
	}
}