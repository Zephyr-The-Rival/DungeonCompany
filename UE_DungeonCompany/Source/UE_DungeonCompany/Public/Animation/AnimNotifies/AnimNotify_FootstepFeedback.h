// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Entities/FootstepSystemComponent.h"
#include "AnimNotify_FootstepFeedback.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UAnimNotify_FootstepFeedback : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
