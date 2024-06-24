// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook/ClimbingHook.h"
#include "Items/ClimbingHook/Rope.h"
#include "WorldActors/Ladder.h"

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

	if (GetHookState() == HookState::InWorldActive || GetHookState() == HookState::InWorldAttached)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		
		SpawnedRope = GetWorld()->SpawnActor<ARope>(RopeClass, GetActorLocation(), GetActorRotation(), spawnParams);

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

void AClimbingHook::CreateLadders(const TArray<FVector>& EdgeLocations)
{
	int locationsNum = EdgeLocations.Num();

	for (int i = 0; i < locationsNum - 1; ++i)
	{
		FVector ladderVector = EdgeLocations[i] - EdgeLocations[i + 1];

		if (ladderVector.Z < MinLadderHeight)
			continue;

		UE_LOG(LogTemp, Warning, TEXT("%s"), *EdgeLocations[i].ToString());

		FVector rightVector = FVector::CrossProduct(ladderVector, FVector::UpVector);

		FVector ladderForwardVector = FVector::CrossProduct(rightVector, ladderVector);

		//if (AttachNormal.Cross(FVector::UpVector).Length() < 0.5)
		//{
		//	ladderForwardVector = edgeLocations[i + 1] - GetActorLocation();
		//	ladderForwardVector.Z = 0.f;
		//}
		//else
		//{
		//	ladderForwardVector = AttachNormal;
		//}

		ALadder* newLadder = GetWorld()->SpawnActor<ALadder>(LadderClass);
		newLadder->SetIgnoreInteractionVolume(true);

		newLadder->PlaceLadder(EdgeLocations[i + 1], EdgeLocations[i], ladderForwardVector);
	}
}

void AClimbingHook::DestroyLadders()
{
	int laddersNum = CreatedLadders.Num();

	for (int i = 0; i < laddersNum; ++i)
	{
		if (!IsValid(CreatedLadders[i]))
			continue;

		CreatedLadders[i]->Destroy();
	}
}