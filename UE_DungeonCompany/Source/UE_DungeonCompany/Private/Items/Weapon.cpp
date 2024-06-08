// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon.h"
#include "DC_Statics.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "Entities/DC_Entity.h"

void AWeapon::BeginPlay()
{
	AWorldItem::BeginPlay();
}

void AWeapon::DealHits_Implementation(UPrimitiveComponent* WeaponCollision, FVector traceStart, FVector TraceEnd)
{
	ADC_Entity* criticallyHitEntity;

	FHitResult hitResult;
	GetWorld()->LineTraceSingleByChannel(hitResult, traceStart, TraceEnd, ECC_GameTraceChannel2);

	if (hitResult.bBlockingHit)
	{
		criticallyHitEntity = Cast<ADC_Entity>(hitResult.GetActor());
	}
	else
	{
		criticallyHitEntity = nullptr;
	}

	//DrawDebugLine(GetWorld(), traceStart, TraceEnd, FColor::Green, false, 1.0f, 0, 1.0f);
		
	TArray<AActor*> overlappingActors;
	WeaponCollision->GetOverlappingActors(overlappingActors);

	for (AActor* a : overlappingActors)
	{
		if (a == this->MyCharacterToAttachTo)
			continue;

		if (Cast<ADC_Entity>(a))//if hit entity
		{
			if (IsValid(criticallyHitEntity) && a == criticallyHitEntity)//was hit on weak spot
			{
				criticallyHitEntity->TakeDamage(20);
			}
			else
			{
				Cast<ADC_Entity>(a)->TakeDamage(10);
			}
		}
		else
		{
			//if hit non-alive thing
		}
	}
}

void AWeapon::TriggerPrimaryAction_Implementation(APlayerCharacter* user)
{
	user->StartAttacking();
}




