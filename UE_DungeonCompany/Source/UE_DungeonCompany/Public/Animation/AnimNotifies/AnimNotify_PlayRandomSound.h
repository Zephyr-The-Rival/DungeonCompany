// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify_PlaySound.h"
#include "AnimNotify_PlayRandomSound.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UAnimNotify_PlayRandomSound : public UAnimNotify_PlaySound
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AnimNotify", meta=(ExposeOnSpawn = true))
	TArray<USoundBase*> Sounds;

private:
	bool bSoundsArraySetup = false;
	TArray<USoundBase*> InactiveSounds;

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
