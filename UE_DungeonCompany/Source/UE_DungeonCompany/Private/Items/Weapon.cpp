// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon.h"
#include "DC_Statics.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "Entities/DC_Entity.h"
#include "NiagaraFunctionLibrary.h"

void AWeapon::BeginPlay()
{
	AWorldItem::BeginPlay();
}

void AWeapon::DealHits_Implementation(UPrimitiveComponent* WeaponCollision, const TArray<FVector>& TraceStarts, const TArray<FVector>&  TraceEnd)
{

	if (TraceStarts.Num() != TraceEnd.Num() || TraceStarts.Num() == 0)
	{
		LogWarning(TEXT("Include all trace starts and ends in correct order"));
		return;
	}

	
	ADC_Entity* hitEntity;
	FName bonename;
	FVector impactPoint;
	for (int i = 0; i < TraceStarts.Num(); i++)
	{
		FHitResult hitResult;
		GetWorld()->LineTraceSingleByChannel(hitResult, TraceStarts[i], TraceEnd[i], ECC_GameTraceChannel4);

		//DrawDebugLine(GetWorld(), TraceStarts[i], TraceEnd[i], FColor::Green, false, 1.0f, 0, 1.0f);
		
		if(!hitResult.bBlockingHit)
			continue;
		
		hitEntity = Cast<ADC_Entity>(hitResult.GetActor());	 	 	 		

		hitEntity->TakeDamage(10);
		hitEntity->SpawnHitEffect(Cast<USceneComponent>(hitResult.GetComponent()), hitResult.BoneName, hitResult.Location, TraceEnd[i] - TraceStarts[i]);
		break;

	}
	


		

	TArray<AActor*> overlappingActors;
	WeaponCollision->GetOverlappingActors(overlappingActors);


	for (AActor* a : overlappingActors)
	{
		//only for non alive things
	}
}

void AWeapon::TriggerPrimaryAction_Implementation(APlayerCharacter* User)
{
	User->StartAttacking();
}

void AWeapon::TriggerLocalPrimaryAction_Implementation(APlayerCharacter* User)
{
	User->AttackStart();
}




