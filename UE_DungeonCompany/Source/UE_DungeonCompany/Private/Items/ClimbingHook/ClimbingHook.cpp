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

	SetActorTickEnabled(HasAuthority());

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

	DestroyLadders();

	Super::EndPlay(EndPlayReason);
}

void AClimbingHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!IsValid(SpawnedRope))
		return;

	if (SpawnedRope->IsRopeSettled())
	{
		TArray<FVector> edgeLocations;
		SpawnedRope->GetEdgeLocations(edgeLocations);

		SpawnedRope->FreezeAndReplicate();

		//CreateLadders(edgeLocations);

		SetActorTickEnabled(false);
	}
}

void AClimbingHook::CreateLadders(const TArray<FVector>& EdgeLocations)
{
	int locationsNum = EdgeLocations.Num();

	for (int i = 0; i < locationsNum - 1; ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *EdgeLocations[i].ToString());

		FVector ladderVector = EdgeLocations[i] - EdgeLocations[i + 1];

		if (ladderVector.Z < MinLadderHeight)
			continue;

		ladderVector.Normalize();

		FVector rightVector = FVector::CrossProduct(ladderVector, FVector::UpVector);
		rightVector.Normalize();
	
		FVector ladderForwardVector;

		FHitResult hit;

		const int steps = 16;

		DrawDebugLine(GetWorld(), EdgeLocations[i + 1], EdgeLocations[i + 1] + ladderVector, FColor::Green, false, 5.f);
		DrawDebugLine(GetWorld(), EdgeLocations[i], EdgeLocations[i] + rightVector * 40, FColor::Red, false, 5.f);

		for (int j = 0; j < steps; ++j)
		{
			FVector start = EdgeLocations[i];

			FVector end = start + rightVector.FVector::RotateAngleAxis((360.f / steps) * j, ladderVector) * 20;
			//FVector end = start + FVector(FMath::Sin(sinSteps * j), FMath::Cos(sinSteps * j), 0) * 20;

			GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_GameTraceChannel3);

			DrawDebugLine(GetWorld(), start, end, FColor::Blue, false, 5.f);

			if(hit.bBlockingHit)
				ladderForwardVector += hit.Normal;
		}

		ladderForwardVector.Normalize();

		ALadder* newLadder = GetWorld()->SpawnActor<ALadder>(LadderClass);
		newLadder->SetIgnoreInteractionVolume(true);

		newLadder->PlaceLadder(EdgeLocations[i + 1], EdgeLocations[i], ladderForwardVector);

		CreatedLadders.Add(newLadder);
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