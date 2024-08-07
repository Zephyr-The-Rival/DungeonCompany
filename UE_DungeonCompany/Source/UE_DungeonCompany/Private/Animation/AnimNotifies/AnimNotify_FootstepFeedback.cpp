// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotify_FootstepFeedback.h"

void UAnimNotify_FootstepFeedback::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && Animation)
	{
		if(const AActor* Owner = MeshComp -> GetOwner())
		{
			if(UFootstepSystemComponent* FootstepSystem = Owner->FindComponentByClass<UFootstepSystemComponent>())
			{
				FootstepSystem -> PlayStepsFeedback();
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MeshComp or Animation is not valid in FootstepAnimNotify."));
	}
}

