// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffSystem/DebuffPoisonGas.h"
#include "PlayerCharacter/PlayerCharacter.h"

void UDebuffPoisonGas::Apply()
{
	Super::Apply();

	APlayerCharacter* playerCharacter = GetOuterEntity<APlayerCharacter>();
	if(!playerCharacter)
		return;

	GetWorld()->GetTimerManager().SetTimer(CoughHandle, playerCharacter, &APlayerCharacter::Cough, CoughInterval, true);

}

void UDebuffPoisonGas::AuthorityApply()
{
	float DamagePerTick = DamageIntervalSeconds / DamagePerSecond;

	FTimerDelegate delegate = FTimerDelegate::CreateUObject(GetOuterEntity(), &ADC_Entity::TakeDamage, DamagePerTick);
	
	GetWorld()->GetTimerManager().SetTimer(DamageHandle, delegate, DamageIntervalSeconds, true);

}

void UDebuffPoisonGas::Remove()
{
	Super::Remove();

	GetWorld()->GetTimerManager().ClearTimer(CoughHandle);

}

void UDebuffPoisonGas::AuthorityRemove()
{
	GetWorld()->GetTimerManager().ClearTimer(DamageHandle);

}
