// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon.h"
#include "DC_Statics.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "WorldActors/BreakableProp.h"
#include "Entities/DC_Entity.h"
#include "NiagaraFunctionLibrary.h"


void AWeapon::BeginPlay()
{
	AWorldItem::BeginPlay();
}

void AWeapon::DealHits(FWeaponInfo WeaponInfo)
{
	
	TArray<FVector> TraceStarts= WeaponInfo.TraceStarts;
	TArray<FVector> TraceEnds= WeaponInfo.TraceEnds;
	
	if (TraceStarts.Num() != TraceEnds.Num() || TraceStarts.Num() == 0)
	{
		LogWarning(TEXT("Do Include all trace starts and ends in correct order!"));
		return;
	}

	
	AActor* HitActor=nullptr;
	FHitResult hitResult;
	int i = 0;
	for (; i < TraceStarts.Num(); i++)
	{

		GetWorld()->LineTraceSingleByChannel(hitResult, TraceStarts[i], TraceEnds[i], ECC_GameTraceChannel4);
		
		//DrawDebugLine(GetWorld(), TraceStarts[i], TraceEnds[i], FColor::Green, false, 1.0f, 0, 1.0f);
		
		if(!hitResult.bBlockingHit)
			continue;
		
		HitActor = hitResult.GetActor();	 	 	 		
		break;
	}

	if(!IsValid(HitActor))
		return;
	
	if(ADC_Entity* hitEntity= Cast<ADC_Entity>(HitActor))
	{
		if(hitEntity!=MyCharacterToAttachTo)
		{
			hitEntity->TakeDamage(10);
			hitEntity->SpawnHitEffect(Cast<USceneComponent>(hitResult.GetComponent()), hitResult.BoneName, hitResult.Location, TraceEnds[i] - TraceStarts[i]);
		}
			
	}

	if (ABreakableProp* Prop = Cast<ABreakableProp>(HitActor))
	{
		Prop->Hit(TraceEnds[i] - TraceStarts[i], this->GetClass());
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




