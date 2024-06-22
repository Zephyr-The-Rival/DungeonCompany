// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook/ClimbingHook.h"
#include "Items/ClimbingHook/Rope.h"

AClimbingHook::AClimbingHook()
{
	bReplicates = true;

}

void AClimbingHook::BeginPlay()
{

	Super::BeginPlay();

	//Rope->SetAttachEndToComponent(RopeEnd);

	if(!HasAuthority())
		return;

	if (State == ClimbingHookState::InWorldActive)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetActorLocation());
		
		SpawnedRope = GetWorld()->SpawnActorDeferred<ARope>(RopeClass, SpawnTransform);
		SpawnedRope->SetAttachingActor(this);
		SpawnedRope->FinishSpawning(SpawnTransform);
	}
		
}

void AClimbingHook::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!HasAuthority())
		return;

	if(IsValid(SpawnedRope))
		SpawnedRope->Destroy();

	Super::EndPlay(EndPlayReason);
}