// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotify_ReportNoiseEvent.h"

#include "Perception/AISense_Hearing.h"

UAnimNotify_ReportNoiseEvent::UAnimNotify_ReportNoiseEvent()
{
	Loudness = 1.f;
}

void UAnimNotify_ReportNoiseEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ReportNoise(MeshComp->GetOwner());
}

void UAnimNotify_ReportNoiseEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ReportNoise(MeshComp->GetOwner());
}

void UAnimNotify_ReportNoiseEvent::ReportNoise(AActor* Owner)
{
	if(!Owner || !Owner->HasAuthority())
		return;

	UAISense_Hearing::ReportNoiseEvent(GetWorld(), Owner->GetActorLocation(), Loudness, Owner);
}
