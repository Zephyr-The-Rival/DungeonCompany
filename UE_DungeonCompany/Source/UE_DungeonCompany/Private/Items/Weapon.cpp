// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon.h"
#include "DC_Statics.h"
#include "PlayerCharacter/PlayerCharacter.h"


TArray<UPrimitiveComponent*> AWeapon::GetHitComponents_Implementation()
{
	LogWarning(TEXT("GetHitActorsWas not overwritten"));
	return TArray<UPrimitiveComponent*>();
}

bool AWeapon::IsWeakspotHit_Implementation()
{
	LogWarning(TEXT("IsWeakspotHit not overwritten"));
	return false;
}

TArray<FWeaponHit> AWeapon::GetHits_Implementation()
{
	LogWarning(TEXT("GetHits_Implementation not overwritten"));
	return TArray<FWeaponHit>();
}

void AWeapon::TriggerPrimaryAction_Implementation(APlayerCharacter* user)
{
	user->AttackStart();
}


