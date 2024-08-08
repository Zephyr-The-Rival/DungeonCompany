// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffSystem/NoSprintDebuff.h"

#include "PlayerCharacter/PlayerCharacter.h"

void UNoSprintDebuff::LocalApply()
{
	Super::LocalApply();

	if(APlayerCharacter* player = Cast<APlayerCharacter>(this->GetOuterEntity()))
	{
		player->bHasNoSprintDebuff=true;
	}
}

void UNoSprintDebuff::LocalRemove()
{
	Super::LocalRemove();
	if(APlayerCharacter* player = Cast<APlayerCharacter>(this->GetOuterEntity()))
	{
		player->bHasNoSprintDebuff=false;
	}
}
