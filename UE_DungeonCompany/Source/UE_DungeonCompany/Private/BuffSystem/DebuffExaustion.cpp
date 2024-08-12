// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffSystem/DebuffExaustion.h"

#include "PlayerCharacter/PlayerCharacter.h"

void UDebuffExaustion::LocalApply()
{
	Super::LocalApply();
	
	if(APlayerCharacter* player = Cast<APlayerCharacter>(this->GetOuterEntity()))
	{
		player->bIsExausted=true;
	}
}

void UDebuffExaustion::LocalRemove()
{
	Super::LocalRemove();
	
	if(APlayerCharacter* player = Cast<APlayerCharacter>(this->GetOuterEntity()))
	{
		player->bIsExausted=false;
	}
}
