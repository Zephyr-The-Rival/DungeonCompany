// Fill out your copyright notice in the Description page of Project Settings.

#include "BuffSystem/DebuffImpairedVision.h"

#include "PlayerCharacter/PlayerCharacter.h"


void UDebuffImpairedVision::LocalApply()
{
	Super::LocalApply();
	if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOuterEntity()))
	{
		UCameraComponent* Camera = player->FirstPersonCamera;
		this->materialIndex = Camera->PostProcessSettings.WeightedBlendables.Array.Add(this->PostProcessMaterial);
		this->SmoothApply(Camera);
	}
}

void UDebuffImpairedVision::LocalRemove()
{
	Super::LocalRemove();
	if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOuterEntity()))
	{
		UCameraComponent* Camera = player->FirstPersonCamera;
		Camera->PostProcessSettings.WeightedBlendables.Array.RemoveAt(this->materialIndex);
	}
	
}

void UDebuffImpairedVision::SmoothApply_Implementation(UCameraComponent* Camera)
{
}
