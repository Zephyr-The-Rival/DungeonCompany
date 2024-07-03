// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ReportNoiseEvent.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UAnimNotify_ReportNoiseEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_ReportNoiseEvent();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(ExposeOnSpawn = true))
	float Loudness;

protected:
	void ReportNoise(AActor* Owner);
	
};
