// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon.h"
#include "DC_Statics.h"


TArray<AActor*> AWeapon::GetHitActors_Implementation()
{
	LogWarning(TEXT("GetHitActorsWas not overwritten"));
	return TArray<AActor*>();
}

